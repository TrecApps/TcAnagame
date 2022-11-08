#include "TVideoControl.h"

TVideoControl::TVideoControl(TrecPointer<DrawingBoard> board, TDataMap<TDataMap<TString>> styles):TControl(board,styles)
{
	targetVideoSet = false;
	targetVideo = 0;
}

TVideoControl::~TVideoControl()
{
}

bool TVideoControl::setVideo(const TString& str)
{
	targetVideoSet = false;
	targetVideo = 0;
	TrecPointer<TFileShell> file = TFileShell::GetFileInfo(str);
	TrecPointer<TVidPlayer> vidPlayer = TVidPlayer::GetPlayer(drawingBoard, file);

	if (!vidPlayer.Get())
		return false;


	if (player.Get())
		player->Stop();
	player = vidPlayer;

	TDataArray<UINT> vids = player->GetVideoStreams();

	if (vids.Size())
	{
		targetVideoSet = true;
		targetVideo = vids[0];
	}
	return true;
}

bool TVideoControl::Play()
{
	if(!player.Get())
	return false;
	tc_async_runner_state s = player->GetCurrentState();
	if (s == tc_async_runner_state::paused)
		player->Resume();
	else
	{
		player->Stop();
		ReturnObject ret;
		player->Run(ret);
	}
}

bool TVideoControl::Pause()
{
	if(!player.Get())
	return false;
	player->Pause();
}

bool TVideoControl::Stop()
{
	if(!player.Get())
	return false;
	player->Stop();
}

bool TVideoControl::Speed(float)
{
	return false;
}

bool TVideoControl::onCreate(const RECT_F& loc, TrecPointer<TFileShell> d)
{
	bool returnable = TControl::onCreate(loc, d);
	targetVideoSet = false;
	targetVideo = 0;
	if (!content.Get())
	{
		content = TControlComponent::GetControlContent(drawingBoard, TrecPointerKey::TrecFromSoft<>(self));
		content->onCreate(loc);
	}
	TString valpoint;
	if (attributes.retrieveEntry(L"MediaSource", valpoint))
	{
		if (!setVideo(valpoint))
			returnable = false;
	}
	return returnable;
}

void TVideoControl::Draw(TrecPointer<TVariable> object)
{
	if (!content.Get() || !player.Get() || !targetVideoSet)
		return;
	content->SetImageBrush(player->RetrieveBrush(targetVideo));
	TControl::Draw(object);
}
