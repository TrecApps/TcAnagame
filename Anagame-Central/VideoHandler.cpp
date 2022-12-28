#include "VideoHandler.h"
#include <AnafacePage.h>
#include <TDialog.h>

TString vidFiles(L"Video Files;avi");

void VideoHandler::Initialize(TrecPointer<TPage> page)
{
    TrecPointer<AnafacePage> mainPage = TrecPointerKey::ConvertPointer<TPage, AnafacePage>(page);
    if (!mainPage.Get())
        return;

    this->playPause = mainPage->GetControlById(L"PlayButton");
    this->videoPlayer = TrecPointerKey::ConvertPointer<TControl, TVideoControl>(mainPage->GetControlById(L"Player"));
    this->board = page->GetDrawingBoard();
    assert(playPause.Get() && videoPlayer.Get());
}

bool VideoHandler::OnChar(bool fromChar, UINT nChar, UINT nRepCnt, UINT nFlags, message_output* mOut)
{
    return false;
}

void VideoHandler::SetSelf(TrecPointer<TPage::EventHandler> handleSelf)
{
}

TrecPointer<TPage> VideoHandler::GetPage()
{
    return TrecPointer<TPage>();
}

void VideoHandler::HandleEvents(TDataArray<TPage::EventID_Cred>& eventAr)
{
    for (UINT Rust = 0; Rust < eventAr.Size(); Rust++)
    {
        auto& args = eventAr[Rust].args;
        if (args.Get())
        {
            if (!args->methodID.Compare(L"PlayPause"))
            {
                video_state state = videoPlayer->GetVideoState();

                if (state == video_state::vs_init || state == video_state::vs_paused)
                {
                    videoPlayer->Play();
                    auto ppText = playPause->GetTextElement();
                    ppText->SetText(L"Pause");
                    board->PrepRefresh();
                }
                else if (state == video_state::vs_playing)
                {
                    videoPlayer->Pause();
                    auto ppText = playPause->GetTextElement();
                    ppText->SetText(L"Play");
                    board->PrepRefresh();
                }
            }
            else if (!args->methodID.Compare(L"SelectVid"))
            {
                TString error;
                TrecPointer<TFileShell> videoFile = OpenLoadFileDialog(vidFiles, TrecPointer<TDirectory>(), error);

                if (videoFile.Get())
                    videoPlayer->setVideo(videoFile->GetPath());
                else {
                    // To-Do: Send Alert Dialog Once One is Available
                }
            }
        }
    }
}
