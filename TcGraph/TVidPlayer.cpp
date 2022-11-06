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
    this->frame = av_frame_alloc();
}
TcAVFrame::TcAVFrame(const TcAVFrame& copy)
{
    this->frame = av_frame_alloc();
    av_frame_copy(this->frame, copy.frame);
    this->brush = copy.brush;
}
TcAVFrame::~TcAVFrame()
{
    av_frame_free(&frame);
}

void TcAVFrame::SetFrame(AVFrame* frame)
{
    av_frame_copy(this->frame, frame);
}

void Stream::ProcessFrames(TrecPointer<DrawingBoard>& board)
{

    if (streamType == av_stream_type::t_video)
    {
        for (UINT Rust = 0; Rust < this->frames.GetSize(); Rust++)
        {
            auto& frame = frames.at(Rust);

            if (frame.brush.Get())
                continue;

            UCHAR* frame_data = (UCHAR*)_aligned_malloc(codec.width * codec.height * 4, 128);
            uint8_t* dest[4] = { frame_data, NULL, NULL, NULL };
            int dest_linesize[4] = { codec.width * 4, 0, 0, 0 };
            sws_scale(codec.scalerContext, frame.frame->data, frame.frame->linesize, 0, codec.height, dest, dest_linesize);

            frame.brush = TrecPointerKey::ConvertPointer<TBrush, TImageBrush>(board->GetImageBrush(frame_data, codec.width, codec.height));

            _aligned_free(frame_data);
        }
        return;
    }
    if (streamType == av_stream_type::t_audio)
    {
        // To-Do: Add Sound support
    }
    
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
    return avFrame && avPacket;
}

bool TVidPlayer::RunRound()
{
    SupplementStreams();

    // Check to see if each Stream needs prcessing
    return false;
}

bool TVidPlayer::SupplementStreams()
{
    if (endOfFile)
        return true;
    int readResult = 0;
    while (NeedsFrames() && (readResult = av_read_frame(avFormatContext, avPacket)) >= 0)
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

            readResult = avcodec_receive_frame(curStream.codec.av_codec_ctx, avFrame);
            if (readResult == AVERROR(EAGAIN) || readResult == AVERROR_EOF) {
                av_packet_unref(avPacket);
                continue;
            }
            else if (readResult < 0) {
                return false;
            }
            TcAVFrame tcFrame;
            tcFrame.SetFrame(avFrame);
            curStream.frames.Push(tcFrame);

            av_packet_unref(avPacket);
        }
    }
    return true;
}


bool TVidPlayer::NeedsFrames()
{
    for (UINT Rust = 0; Rust < streams.Size(); Rust++)
    {
        if (streams[Rust].frames.GetSize() < frameBuffer)
            return true;
    }
    return false;
}

TrecPointer<TVariable> TVidPlayer::Clone()
{
    return TrecPointer<TVariable>();
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

TrecPointer<TVidPlayer> TVidPlayer::GetPlayer(TrecPointer<DrawingBoard> board, TrecPointer<TFileShell> file, UINT frameBufferDefault)
{
    if (!board.Get())
        return TrecPointer<TVidPlayer>();
    AVFormatContext* avFormatContext = avformat_alloc_context();

    TrecPointer<TVidPlayer> ret;

    if (!avFormatContext)
        return ret;

    if (avformat_open_input(&avFormatContext, file->GetPath().GetRegString().c_str(), nullptr, nullptr))
    {
        avFormatContext = nullptr; // Function call automatically deallocates context upon failure
        return ret;
    }

    ret = TrecPointerKey::ConvertPointer<TVariable, TVidPlayer>(TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TVidPlayer>());
    ret->board = board;
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

        Stream stream;

        if (av_codec_params->codec_type == AVMEDIA_TYPE_AUDIO) {
            stream.codec = { av_codec_params, av_codec, 0,0, codecContext, avFormatContext->streams[Rust]->time_base, Rust, nullptr };
            stream.streamType = av_stream_type::t_audio;
        }
        if (av_codec_params->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            SwsContext* context = nullptr;

            auto source_pix_fmt = correct_for_deprecated_pixel_format(codecContext->pix_fmt);

            context = sws_getContext(
                av_codec_params->width, 
                av_codec_params->height,
                source_pix_fmt,
                av_codec_params->width,
                av_codec_params->height,
                AV_PIX_FMT_RGB0,
                SWS_BILINEAR, NULL, NULL, NULL);

            if (!context)
            {
                avcodec_free_context(&codecContext);
                codecContext = nullptr;
                continue;
            }

            stream.codec = {
            av_codec_params,
            av_codec,
            av_codec_params->width,
            av_codec_params->height,
            codecContext,
            avFormatContext->streams[Rust]->time_base, Rust, context };
            stream.streamType = av_stream_type::t_video;
        }

        ret->streams.push_back(stream);
    }

    ret->frameBuffer = frameBufferDefault;
    ret->endOfFile = false;

    if (!ret->Initialize())
        ret.Nullify();
    return ret;

}
