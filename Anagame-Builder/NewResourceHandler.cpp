#include "NewResourceHandler.h"

TString onOkay(L"OnOkay");
TString onCancel(L"OnCancel");

TString onUpdateName(L"OnUpdateName");
TString onSelectRecent(L"OnResourceSelect");

NewResourceHandler::NewResourceHandler()
{
    calls.push_back(&NewResourceHandler::OnOkay);
    events.addEntry(onOkay, 0);

    calls.push_back(&NewResourceHandler::OnCancel);
    events.addEntry(onCancel, 1);

    calls.push_back(&NewResourceHandler::OnUpdateName);
    events.addEntry(onUpdateName, 2);

    calls.push_back(&NewResourceHandler::OnResourceSelect);
    events.addEntry(onSelectRecent, 3);
}

void NewResourceHandler::Initialize(TrecPointer<TPage> page)
{
}

bool NewResourceHandler::OnChar(bool fromChar, UINT nChar, UINT nRepCnt, UINT nFlags, message_output* mOut)
{
    return false;
}

void NewResourceHandler::SetSelf(TrecPointer<TPage::EventHandler> handleSelf)
{
}

TrecPointer<TPage> NewResourceHandler::GetPage()
{
    return TrecPointer<TPage>();
}


void NewResourceHandler::Close()
{
    //inputDirectory.Nullify();
    //inputName.Nullify();
    auto tempDialogWindow = dialogWindow;
    dialogWindow.Nullify();

    TInstance::GetInstance()->OnWindowClose(tempDialogWindow->GetGlfwWindow());
    glfwDestroyWindow(tempDialogWindow->GetGlfwWindow());
}

void NewResourceHandler::HandleEvents(TDataArray<TPage::EventID_Cred>& eventAr)
{
    int e_id = -1;
    EventArgs ea;
    for (UINT c = 0; c < eventAr.Size(); c++)
    {
        auto tc = eventAr.at(c).args;

        if (!tc.Get()) continue;

        UINT u_id = 0;
        if (!events.retrieveEntry(tc->methodID, u_id))
            continue;
        e_id = u_id;
        // At this point, call the appropriate method
        if (e_id > -1 && e_id < calls.Size())
        {

            ea = *tc.Get();
            // call method
            if (calls[e_id])
                (this->*calls[e_id])(eventAr[c].control, ea);
        }
        eventAr[c].args.Nullify();
    }
}

void NewResourceHandler::OnCancel(TrecPointer<TPage> tc, EventArgs ea)
{
}

void NewResourceHandler::OnOkay(TrecPointer<TPage> tc, EventArgs ea)
{
}

void NewResourceHandler::OnUpdateName(TrecPointer<TPage> tc, EventArgs ea)
{
}

void NewResourceHandler::OnResourceSelect(TrecPointer<TPage> tc, EventArgs ea)
{
}
