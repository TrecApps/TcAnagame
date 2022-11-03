#pragma once
#include <TcRunner.h>
#include "DrawingBoard.h"



class StoryBoard :
    public TcAsyncRunner
{
	TDataArray<TrecPointer<Animation>> animations;
	animation_mode animationMode;
	UINT refreshRate;
	UINT totalLength;
	UINT runCount;

	ULONG64 timer;
	LONG64 lag;
public:
	bool AddAnimation(TrecPointer<Animation> animation);
	StoryBoard(animation_mode mode = animation_mode::run_once);

	bool SetAnimationMode(animation_mode mode);
	void Run(ReturnObject& ret)override;
protected:
	bool RunRound()override;
	bool DoReset();
};

