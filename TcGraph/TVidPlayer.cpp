#include "TVidPlayer.h"
#include <cstdlib>

static AVPixelFormat correct_for_deprecated_pixel_format(AVPixelFormat pix_fmt) {
    // Fix swscaler deprecated pixel format warning
    // (YUVJ has been deprecated, change pixel format to regular YUV)
    switch (pix_fmt) {
    case AV_PIX_FMT_YUVJ420P: return AV_PIX_FMT_YUV420P;
    case AV_PIX_FMT_YUVJ422P: return AV_PIX_FMT_YUV422P;
    case AV_PIX_FMT_YUVJ444P: return AV_PIX_FMT_YUV444P;
    case AV_PIX_FMT_YUVJ440P: return AV_PIX_FMT_YUV440P;
    default:                  return pix_fmt;
    }
}

TcAVFrame::TcAVFrame()
{
    this->frame = nullptr;
    this->createdOp = false;
}
TcAVFrame::~TcAVFrame()
{
    av_frame_free(&frame);
}

TrecPointer<TImageBrush> TcAVFrame::GetBrush()
{
    TObjectLocker lock(&thread);
    return this->brush;
}

void TcAVFrame::SetBrush(TrecPointer<TImageBrush> brush)
{
    if (brush.Get()) {
        TObjectLocker lock(&thread);
        this->brush = brush;
    }
}


int TcAVFrame::RecieveFrame(AVCodecContext* avctx)
{
    if (!frame)
        frame = av_frame_alloc();
    return avcodec_receive_frame(avctx, frame);
}

AVFrame* TcAVFrame::GetFrame()
{
    return this->frame;
}

void Stream::PrepCodec()
{
    if (streamType == av_stream_type::t_video && !codec.scalerContext)
    {
        auto source_pix_fmt = correct_for_deprecated_pixel_format(codec.av_codec_ctx->pix_fmt);
        codec.scalerContext = sws_getContext(
            codec.width, codec.height, source_pix_fmt,
            codec.width, codec.height, AV_PIX_FMT_RGB0,
            SWS_BILINEAR, nullptr, nullptr, nullptr);
    }
}

void Stream::ProcessFrames(TrecPointer<DrawingBoard>& board)
{

    if (streamType == av_stream_type::t_video)
    {
        if (!codec.scalerContext)
        {
            /*PrepCodec();
            if(!codec.scalerContext)*/
                return;
        }
        for (auto Rust = this->frames.begin(); Rust != frames.end(); Rust++)
        {
            auto& frame = (*Rust);

            if (frame->brush.Get())
                continue;

            
        }
        return;
    }
    if (streamType == av_stream_type::t_audio)
    {
        // To-Do: Add Sound support
    }
    
}

bool Stream::DoPresent(double& baseTime)
{
    double current = glfwGetTime();
    double timeBase = static_cast<double>(codec.timeBase.num) / static_cast<double>(codec.timeBase.den);

    bool ret = false;

    this->dropCount = 0;

    for (auto Rust = frames.begin(); Rust != frames.end(); Rust++)
    {
        auto& frame = *Rust;
        if (!timestampCorrection && frame->frame->pts < 0)
            timestampCorrection = -static_cast<double>(frame->frame->pts);

        bool doPresent = ((timestampCorrection + (static_cast<double>(frame->frame->pts)) * timeBase) + baseTime) > current;
        if (!doPresent)
        {
            if (frames.size() > 20)
                dropCount = frames.size() - 20;
            return ret;
        }
        if (this->streamType == av_stream_type::t_video && frame->GetBrush().Get())
        {
            TObjectLocker lock(&thread);
            brush = frame->GetBrush();
            ret = true;
            dropCount++;
        }
        else if (streamType == av_stream_type::t_audio)
        {
            dropCount++;
        }
        
        
    }
    return ret;
}

void Stream::DropFrames()
{
    for (UINT Rust = 0; Rust < dropCount; Rust++)
    {
        frames.pop_front();
    }
    dropCount = 0;
}

bool TVidPlayer::Initialize()
{
    avFrame = nullptr;
    avPacket = nullptr;
    
    for (UINT Rust = 0; Rust < streams.Size(); Rust++)
    {
        AVCodecContext* av_codec_ctx = avcodec_alloc_context3(streams[Rust].codec.av_codec);
        if (!av_codec_ctx || avcodec_parameters_to_context(av_codec_ctx, streams[Rust].codec.av_codec_params) < 0)
        {
            streams.RemoveAt(Rust--);
            continue;
        }
    }

    avFrame = av_frame_alloc();
    avPacket = av_packet_alloc();
    bool worked = avFrame && avPacket;
    if (worked)
        videoState = video_state::vs_init;
    return worked;
}

bool TVidPlayer::RunRound()
{
    if (!SupplementStreams()) 
        return true;

    // Check to see if each Stream needs prcessing
    bool ret = true;

    
    for (UINT Rust = 0; Rust < streams.Size(); Rust++)
    {
        streams[Rust].ProcessFrames(board);
        if (streams[Rust].DoPresent(baseTime))
            board->PrepRefresh();
        if (streams[Rust].frames.size())
            ret = false;
        streams[Rust].DropFrames();
    }

    if (ret)
        videoState = video_state::vs_stopped;
    return ret;
}

bool TVidPlayer::SupplementStreams()
{
    if (endOfFile)
        return true;
    int readResult = 0;
    while (NeedsFrames() && ((readResult = av_read_frame(avFormatContext, avPacket)) >= 0))
    {
        // End of File, this will cause the Async Runer to enter a "finished" state
        if (readResult < 0)
        {
            return endOfFile = true;
        }

        if (avPacket->stream_index < streams.Size())
        {
            auto& curStream = streams[avPacket->stream_index];
            
            readResult = avcodec_send_packet(curStream.codec.av_codec_ctx, avPacket);
            if (readResult < 0) {
                return false;
            }
            TrecPointer<TcAVFrame> tcFrame = TrecPointerKey::GetNewTrecPointer<TcAVFrame>();
            readResult = tcFrame->RecieveFrame(curStream.codec.av_codec_ctx);
            if (readResult == AVERROR(EAGAIN) || readResult == AVERROR_EOF) {
                av_packet_unref(avPacket);
                continue;
            }
            else if (readResult < 0) {
                return false;
            }
            

            
            curStream.frames.push_back(tcFrame);
            curStream.PrepCodec();
            if (curStream.streamType == av_stream_type::t_video) {
                // If this is a video go ahead and signal the drawing Board to 
                board->AddOperation(TrecPointerKey::GetNewTrecPointerAlt<DrawingBoard::GraphicsOp, TextureToAVFrameOperation>(
                    tcFrame,
                    curStream.codec.scalerContext,
                    curStream.codec.width,
                    curStream.codec.height
                ));
            }

            av_packet_unref(avPacket);
            //av_packet_free(&avPacket);
        }
    }
    return true;
}


bool TVidPlayer::NeedsFrames()
{
    for (UINT Rust = 0; Rust < streams.Size(); Rust++)
    {
        if (streams[Rust].frames.size() < frameBuffer)
            return true;
    }
    return false;
}

TrecPointer<TVariable> TVidPlayer::Clone()
{
    return TrecPointer<TVariable>();
}

void TVidPlayer::Run(ReturnObject& ret)
{
    if (videoState == video_state::vs_init || videoState == video_state::vs_stopped)
    {
        baseTime = glfwGetTime();
        TcAsyncRunner::Run(ret);
        videoState = video_state::vs_playing;
        if (board.Get())
            constantRefreshIndex = board->PrepConstantRefresh();
    }
}

void TVidPlayer::Pause()
{
    if (videoState == video_state::vs_playing)
    {
        TcAsyncRunner::Pause();
        pauseTime = glfwGetTime();
        videoState = video_state::vs_paused;
        if (board.Get())
            board->ReleaseConstantRefresh(constantRefreshIndex);
    }
}

void TVidPlayer::Resume()
{
    if (videoState == video_state::vs_paused)
    {
        baseTime = (glfwGetTime() - pauseTime);
        TcAsyncRunner::Resume();
        videoState = video_state::vs_playing;
        if (board.Get())
            constantRefreshIndex = board->PrepConstantRefresh();
    }
}

void TVidPlayer::Stop()
{
    videoState = video_state::vs_stopped;
    TcAsyncRunner::Stop();

    if (board.Get())
        board->ReleaseConstantRefresh(constantRefreshIndex);
}

video_state TVidPlayer::GetVideoState()
{
    return videoState;
}

TVidPlayer::~TVidPlayer()
{
    avformat_close_input(&avFormatContext);
    avformat_free_context(avFormatContext);

    if (avFrame)
        av_frame_free(&avFrame);
    if (avPacket)
        av_packet_free(&avPacket);

    for (UINT Rust = 0; Rust < streams.Size(); Rust++)
    {
        if (streams[Rust].codec.av_codec_ctx)
            avcodec_free_context(&streams[Rust].codec.av_codec_ctx);
    }
}

TDataArray<UINT> TVidPlayer::GetVideoStreams()
{
    TDataArray<UINT> ret;
    for (UINT Rust = 0; Rust < streams.Size(); Rust++)
    {
        if (streams[Rust].streamType == av_stream_type::t_video)
            ret.push_back(Rust);
    }
    return ret;
}


TrecPointer<TImageBrush> TVidPlayer::RetrieveBrush(UINT streamIndex)
{
    if (streamIndex < streams.Size() && streams[streamIndex].streamType == av_stream_type::t_video)
        return streams[streamIndex].brush;
    return TrecPointer<TImageBrush>();
}

TrecPointer<TVidPlayer> TVidPlayer::GetPlayer(TrecPointer<DrawingBoard> board, TrecPointer<TFileShell> file, UINT frameBufferDefault)
{
    if (!board.Get())
        return TrecPointer<TVidPlayer>();
    AVFormatContext* avFormatContext = avformat_alloc_context();

    TrecPointer<TVidPlayer> ret;

    if (!avFormatContext)
        return ret;
    avFormatContext->avoid_negative_ts = AVFMT_AVOID_NEG_TS_MAKE_ZERO;
    if (avformat_open_input(&avFormatContext, file->GetPath().GetRegString().c_str(), nullptr, nullptr))
    {
        avFormatContext = nullptr; // Function call automatically deallocates context upon failure
        return ret;
    }
    
    // 

    ret = TrecPointerKey::ConvertPointer<TVariable, TVidPlayer>(TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TVidPlayer>());
    ret->board = board;
    ret->videoState = video_state::vs_not_init;
    for (UINT Rust = 0; Rust < avFormatContext->nb_streams; Rust++)
    {
        AVCodecParameters* av_codec_params = avFormatContext->streams[Rust]->codecpar;
        const AVCodec* av_codec = avcodec_find_decoder(av_codec_params->codec_id);

        if (!av_codec)
            continue;

        AVCodecContext* codecContext = avcodec_alloc_context3(av_codec);
        if (!codecContext)
        {
            continue;
        }

        if (avcodec_parameters_to_context(codecContext, av_codec_params) < 0) {
            avcodec_free_context(&codecContext);
            codecContext = nullptr;
            continue;
        }

        if (avcodec_open2(codecContext, av_codec, nullptr) < 0) {
            avcodec_free_context(&codecContext);
            codecContext = nullptr;
            continue;
        }

        Stream stream;

        if (av_codec_params->codec_type == AVMEDIA_TYPE_AUDIO) {
            stream.codec = { av_codec_params, av_codec, 0,0, codecContext, avFormatContext->streams[Rust]->time_base, Rust, nullptr };
            stream.streamType = av_stream_type::t_audio;
        }
        if (av_codec_params->codec_type == AVMEDIA_TYPE_VIDEO)
        {

            stream.codec = {
            av_codec_params,
            av_codec,
            av_codec_params->width,
            av_codec_params->height,
            codecContext,
            avFormatContext->streams[Rust]->time_base, Rust, nullptr };
            stream.streamType = av_stream_type::t_video;


        }

        ret->streams.push_back(stream);
    }

    ret->frameBuffer = frameBufferDefault;
    ret->endOfFile = false;

    ret->pauseTime = ret->baseTime = 0.0f;

    if (!ret->Initialize())
        ret.Nullify();

    ret->avFormatContext = avFormatContext;
    return ret;

}

TextureToAVFrameOperation::TextureToAVFrameOperation(TrecPointer<TcAVFrame> frame, SwsContext* context, int w, int h)
{
    this->context = context;
    this->frame = frame;
    this->codecHeight = h;
    this->codecWidth = w;
}

void TextureToAVFrameOperation::Perform(TrecPointer<DrawingBoard> board)
{
    UCHAR* frame_data = (UCHAR*)_aligned_malloc(codecWidth * codecHeight * 4, 128);
    uint8_t* dest[4] = { frame_data, NULL, NULL, NULL };
    int dest_linesize[4] = { codecWidth * 4, 0, 0, 0 };

    frame->ThreadLock();
    AVFrame* rawFrame = frame->GetFrame();

    sws_scale(context, rawFrame->data, rawFrame->linesize, 0, codecHeight, dest, dest_linesize);

    frame->SetBrush(TrecPointerKey::ConvertPointer<TBrush, TImageBrush>(board->GetImageBrush(frame_data, codecWidth, codecHeight, GL_RGB)));

    _aligned_free(frame_data);

    frame->ThreadRelease();
}
