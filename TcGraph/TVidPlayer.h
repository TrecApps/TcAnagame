#pragma once
#include <TcRunner.h>
#include <TFile.h>
#include <TLinkedList.h>
#include "TImageBrush.h"
#include <list>

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

class _TC_GRAPH TcAVFrame: public TObject
{
    friend class TVidPlayer;
    friend class Stream;
    mutable AVFrame* frame;
    TrecPointer<TImageBrush> brush;
    bool createdOp;
public:
    TcAVFrame();
    ~TcAVFrame();

    TrecPointer<TImageBrush> GetBrush();
    void SetBrush(TrecPointer<TImageBrush> brush);


    int RecieveFrame(AVCodecContext* avctx);

    AVFrame* GetFrame();
};

class _TC_GRAPH Stream : public TObject
{
    friend class TVidPlayer;
    av_stream_type streamType;
    TAvCodec codec;
    std::list<TrecPointer<TcAVFrame>> frames;
    UINT dropCount;
    TrecPointer<TImageBrush> brush;

    double timestampCorrection;
public:
    Stream() = default;
    Stream(const Stream& copy) = default;

    void PrepCodec();

    void ProcessFrames(TrecPointer<DrawingBoard>& board);
    bool DoPresent(double& baseTime);

    void DropFrames();
};

class TextureToAVFrameOperation : public DrawingBoard::GraphicsOp {
    TrecPointer<TcAVFrame> frame;
    SwsContext* context;
    int codecWidth, codecHeight;
public:
    TextureToAVFrameOperation(TrecPointer<TcAVFrame> frame, SwsContext* context, int w, int h);
    void Perform(TrecPointer<DrawingBoard> board) override;
};

using tc_player_state = enum class tc_player_state
{

};

using video_state = enum class video_state {
    vs_not_init,
    vs_init,
    vs_playing,
    vs_paused,
    vs_stopped
};

class _TC_GRAPH TVidPlayer :
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

    video_state videoState;

    UINT constantRefreshIndex;
protected:
    bool RunRound() override;

    bool SupplementStreams();
    bool NeedsFrames();

    bool ProcessVideoStream(UINT index);
    bool ProcessAudioStream(UINT index);


public:
    video_state GetVideoState();

    TrecPointer<TVariable> Clone()override;

    void Run(ReturnObject& ret)override;

    void Pause()override;

    void Resume()override;

    void Stop() override;

    ~TVidPlayer()override;

    TDataArray<UINT> GetVideoStreams();

    TrecPointer<TImageBrush> RetrieveBrush(UINT streamIndex);

    static TrecPointer<TVidPlayer> GetPlayer(TrecPointer<DrawingBoard> board, TrecPointer<TFileShell> file, UINT frameBufferDefault = 10);
};

