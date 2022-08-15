#include "TVidPlayer.h"

bool TVidPlayer::Initialize()
{
    avFrame = nullptr;
    avPacket = nullptr;
    
    for (UINT Rust = 0; Rust < audioStreams.Size(); Rust++)
    {
        AVCodecContext* av_codec_ctx = avcodec_alloc_context3(audioStreams[Rust].av_codec);
        if (!av_codec_ctx || avcodec_parameters_to_context(av_codec_ctx, audioStreams[Rust].av_codec_params) < 0)
        {
            audioStreams.RemoveAt(Rust--);
            continue;
        }
    }

    for (UINT Rust = 0; Rust < videoStreams.Size(); Rust++)
    {
        AVCodecContext* av_codec_ctx = avcodec_alloc_context3(videoStreams[Rust].av_codec);
        if (!av_codec_ctx || avcodec_parameters_to_context(av_codec_ctx, videoStreams[Rust].av_codec_params) < 0)
        {
            videoStreams.RemoveAt(Rust--);
            continue;
        }
    }

    avFrame = av_frame_alloc();
    avPacket = av_packet_alloc();
    return avFrame && avPacket;
}

TVidPlayer::~TVidPlayer()
{
    avformat_close_input(&avFormatContext);
    avformat_free_context(avFormatContext);

    if (avFrame)
        av_frame_free(&avFrame);
    if (avPacket)
        av_packet_free(&avPacket);

    for (UINT Rust = 0; Rust < audioStreams.Size(); Rust++)
    {
        if (audioStreams[Rust].av_codec_ctx)
            avcodec_free_context(&audioStreams[Rust].av_codec_ctx);
    }
    for (UINT Rust = 0; Rust < videoStreams.Size(); Rust++)
    {
        if (videoStreams[Rust].av_codec_ctx)
            avcodec_free_context(&videoStreams[Rust].av_codec_ctx);
    }
}

TrecPointer<TVidPlayer> TVidPlayer::GetPlayer(TrecPointer<TFileShell> file)
{
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

    for (UINT Rust = 0; Rust < avFormatContext->nb_streams; Rust++)
    {
        AVCodecParameters* av_codec_params = avFormatContext->streams[Rust]->codecpar;
        AVCodec* av_codec = avcodec_find_decoder(av_codec_params->codec_id);
        if (!av_codec)
            continue;

        if (av_codec_params->codec_type == AVMEDIA_TYPE_AUDIO)
            ret->audioStreams.push_back({ av_codec_params, av_codec, 0,0, nullptr, avFormatContext->streams[Rust]->time_base });
        if (av_codec_params->codec_type == AVMEDIA_TYPE_VIDEO && !ret->videoStreams.Size())
            ret->videoStreams.push_back({ 
            av_codec_params,
            av_codec, 
            av_codec_params->width,
            av_codec_params->height,
            nullptr,
            avFormatContext->streams[Rust]->time_base });
    }


    if (!ret->Initialize())
        ret.Nullify();
    return ret;

}
