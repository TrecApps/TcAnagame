#include "StoryBoard.h"


bool StoryBoard::AddAnimation(TrecPointer<Animation> animation)
{
	if (!DoReset() || !animation.Get())
		return false;
	// If there is an Active Animation underway, don't add new animations

	UINT newRefresh = animation->GetMillisecondRefresh();
	UINT newLength = animation->GetMillisecondLength();

	if (newLength && newRefresh < 10000)
	{
		if (newRefresh && newRefresh < this->refreshRate)
			refreshRate = newRefresh;
		if (newLength > this->totalLength)
			totalLength = newLength;

		animations.push_back(animation);
	}
}

StoryBoard::StoryBoard(animation_mode mode)
{
	SetAnimationMode(mode);
	this->refreshRate = UINT_MAX;
	this->totalLength = 0;
	this->lag = this->timer = 0;
}

bool StoryBoard::SetAnimationMode(animation_mode mode)
{
	if (!DoReset())
		return false;

	this->animationMode = mode;
	return true;
}

void StoryBoard::Run(ReturnObject& ret)
{
	if (!DoReset())
		return;
	this->lag = this->timer = 0;
	this->timer = GetTimeMilli();
	TcAsyncRunner::Run(ret);
}

bool StoryBoard::RunRound()
{

	float progress = 0.0f;
	
	do
	{
		for (UINT Rust = 0; Rust < animations.Size(); Rust++)
		{
			animations[Rust]->Update(progress);
		}

		progress += (static_cast<float>(refreshRate) / static_cast<float>(totalLength));

		lag = (GetTimeMilli() - timer) - refreshRate;

#ifdef _WINDOWS
		Sleep(refreshRate - lag);
#else
		sleep(refreshRate - lag);
#endif // _WINDOWS

		lag = (GetTimeMilli() - timer) - refreshRate;
	} while (progress < 0.9999f);

	switch (animationMode)
	{
	case animation_mode::run_once:
	case animation_mode::forward_loop:
		return animationMode == animation_mode::run_once;
	default:
		for (UINT Rust = 0; Rust < animations.Size(); Rust++)
		{
			auto& ani = animations[Rust];
			ani->reverse = !ani->reverse;
		}
	}

	timer = GetTimeMilli();

	UINT times = this->runCount++;
	if (runCount == 10)
		runCount = 0;

	return animationMode == animation_mode::forward_reverse_once ? (times) : false;

}

bool StoryBoard::DoReset()
{
	if (!bridge.Get())
		return true;

	thread_state tState = bridge->GetState();

	if (tState != thread_state::finished)
		return false;
	bridge.Nullify();
	return true;
}