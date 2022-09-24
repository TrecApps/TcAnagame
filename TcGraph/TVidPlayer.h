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
    const AVCodec* av_codec;
    int width;
    int height;
    AVCodecContext* av_codec_ctx;
    AVRational timeBase;
};

class TVidPlayer :
    public TcRunner
{
    friend class TrecPointerKey;

    AVFormatContext* avFormatContext;
    AVFrame* avFrame;
    AVPacket* avPacket;

    TVidPlayer() = default;
    bool Initialize();

    TDataArray<TAvCodec> videoStreams;
    TDataArray<TAvCodec> audioStreams;

protected:
    void RunDetails(ReturnObject& ret) override;
public:
    TrecPointer<TVariable> Clone()override;
    runner_block_mode GetBlockMode()override;
    bool SetAsync() override;



    ~TVidPlayer()override;

    static TrecPointer<TVidPlayer> GetPlayer(TrecPointer<TFileShell> file);

};

