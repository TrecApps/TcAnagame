#pragma once
#include <TcRunner.h>
#include <TFile.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <inttypes.h>
}

using TAvCodec = struct TAvCodec {
    AVCodecParameters* av_codec_params;
    AVCodec* av_codec;
    int width;
    int height;
    AVCodecContext* av_codec_ctx;
    AVRational timeBase;
};

class TVidPlayer :
    public TcRunner
{
    AVFormatContext* avFormatContext;
    AVFrame* avFrame;
    AVPacket* avPacket;

    TVidPlayer() = default;
    bool Initialize();

    TDataArray<TAvCodec> videoStreams;
    TDataArray<TAvCodec> audioStreams;
public:

    ~TVidPlayer()override;

    static TrecPointer<TVidPlayer> GetPlayer(TrecPointer<TFileShell> file);

};
