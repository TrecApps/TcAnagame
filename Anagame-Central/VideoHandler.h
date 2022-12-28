#pragma once
#include <TControl.h>
#include <TVideoControl.h>

class VideoHandler :
    public TPage::EventHandler
{
public:
	virtual void Initialize(TrecPointer<TPage> page) override;

	virtual bool OnChar(bool fromChar, UINT nChar, UINT nRepCnt, UINT nFlags, message_output* mOut) override;

	virtual void SetSelf(TrecPointer<TPage::EventHandler> handleSelf) override;

	virtual TrecPointer<TPage> GetPage() override;

	virtual void HandleEvents(TDataArray<TPage::EventID_Cred>& eventAr) override;

private:
	TrecPointer<TControl> playPause;
	TrecPointer<TVideoControl> videoPlayer;
	TrecPointer<DrawingBoard> board;
};

