#pragma once
#include "TControl.h"
#include <TVidPlayer.h>



class _ANA_WIDGET TVideoControl :
    public TControl
{
    TrecPointer<TVidPlayer> player;
	bool targetVideoSet;
	UINT targetVideo;
public:
	TVideoControl(TrecPointer<DrawingBoard>, TDataMap<TDataMap<TString>> styles);
	~TVideoControl();
	bool setVideo(const TString& str);
	bool Play();
	bool Pause();
	bool Stop();
	bool Speed(float);
	virtual bool onCreate(const RECT_F& loc, TrecPointer<TFileShell> d)override;

	video_state GetVideoState();

	virtual void Draw(TrecPointer<TVariable> object) override;
};

