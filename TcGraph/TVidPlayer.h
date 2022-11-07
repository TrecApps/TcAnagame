#pragma once
#include <TcRunner.h>
#include <TFile.h>
#include <TLinkedList.h>
#include "TImageBrush.h"

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
    UINT packetIndex;
    SwsContext* scalerContext;
};

using av_stream_type = enum class av_stream_type {
    t_video,
    t_audio
};

class TcAVFrame
{
    friend class TVidPlayer;
    friend class Stream;
    mutable AVFrame* frame;
    TrecPointer<TImageBrush> brush;
public:
    TcAVFrame();
    TcAVFrame(const TcAVFrame& copy);
    ~TcAVFrame();

    void SetFrame(AVFrame* frame);
};

class Stream
{
    friend class TVidPlayer;
    av_stream_type streamType;
    TAvCodec codec;
    TLinkedList<TcAVFrame> frames;
    TrecPointer<TImageBrush> brush;
public:
    Stream() = default;
    Stream(const Stream& copy) = default;

    void ProcessFrames(TrecPointer<DrawingBoard>& board);
    void DoPresent(double& baseTime);
};

class TVidPlayer :
    public TcAsyncRunner
{
    friend class TrecPointerKey;

    TrecPointer<DrawingBoard> board;

    AVFormatContext* avFormatContext;
    AVFrame* avFrame;
    AVPacket* avPacket;

    TVidPlayer() = default;
    bool Initialize();

    TDataArray<Stream> streams;

    UINT frameBuffer;           // minimum number of frams for each stream to hold in reserve
    bool endOfFile;

    double baseTime;
    double pauseTime;
protected:
    bool RunRound() override;

    bool SupplementStreams();
    bool NeedsFrames();

    bool ProcessVideoStream(UINT index);
    bool ProcessAudioStream(UINT index);


public:
    TrecPointer<TVariable> Clone()override;

    void Run(ReturnObject& ret)override;

    void Pause()override;

    void Resume()override;

    ~TVidPlayer()override;

    static TrecPointer<TVidPlayer> GetPlayer(TrecPointer<DrawingBoard> board, TrecPointer<TFileShell> file, UINT frameBufferDefault = 10);
};

