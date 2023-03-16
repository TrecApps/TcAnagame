#include "NewResourceHandler.h"
#include <AnafacePage.h>


NewResourceHandler::NewResourceHandler()
{
    calls.push_back(&NewResourceHandler::OnOkay);
    events.addEntry(L"OnOkay", 0);

    calls.push_back(&NewResourceHandler::OnCancel);
    events.addEntry(L"OnCancel", 1);

    calls.push_back(&NewResourceHandler::OnUpdateName);
    events.addEntry(L"OnUpdateName", 2);

    calls.push_back(&NewResourceHandler::OnResourceSelect);
    events.addEntry(L"OnResourceSelect", 3);

    nameRequired = false;
}

void NewResourceHandler::Initialize(TrecPointer<TPage> page)
{
    dialogWindow = TrecPointerKey::ConvertPointer<DrawingBoard, TDialog>(page->GetDrawingBoard());
    assert(dialogWindow.Get());

    auto tempWindow = TrecPointerKey::ConvertPointer<TWindow, TIdeWindow>(dialogWindow->GetParentWindow());

    window = TrecPointerKey::SoftFromTrec<TIdeWindow>(tempWindow);

    auto env = tempWindow->GetEnvironment();

    TDataArray<TrecPointer<TVariable>> resourceDataList;
    env->RetrieveResourceList(resourceDataList);

    auto actList = TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TArrayVariable>();
    resourceList = TrecPointerKey::ConvertPointer<TVariable, TArrayVariable>(actList);
    for (UINT Rust = 0; Rust < resourceDataList.Size(); Rust++)
    {
        resourceList->Push(resourceDataList[Rust]);
    }
    auto anaPage = TrecPointerKey::ConvertPointer<TPage, AnafacePage>(page);
    resourceLayout = TrecPointerKey::ConvertPointer<TControl, TDataLayout>(anaPage->GetControlById(L"ResourceList"));
    resourceLayout->SetData(actList);

    auto control = anaPage->GetControlById(L"OkButton");
    okayButton = TrecPointerKey::SoftFromTrec<>(control);

    auto textControl = TrecPointerKey::ConvertPointer<TControl, TTextInput>(anaPage->GetControlById(L"OnUpdateName"));
    nameInput = TrecPointerKey::SoftFromTrec<>(textControl);

    AssessOkay();
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

void NewResourceHandler::AssessOkay()
{
    bool works = true;
    auto actNameInput = TrecPointerKey::TrecFromSoft<>(nameInput);

    if (!resourceSource.GetSize())
        works = false;
    if (works && resourceTitle.GetSize())
        works = false;
    if (works && (!nameRequired || !actNameInput.Get() || !actNameInput->GetTextElement()->GetText()->GetSize()))
        works = false;

    auto oButton = TrecPointerKey::TrecFromSoft<>(okayButton);
    oButton->setActive(works);

    dialogWindow->PrepRefresh();
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
    Close();
}

void NewResourceHandler::OnOkay(TrecPointer<TPage> tc, EventArgs ea)
{
    if (dialogWindow.Get())
    {
        TrecPointer<TVariable> proj = TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TJsonVariable>();
        TrecPointer<TJsonVariable> jProj = TrecPointerKey::ConvertPointer<TVariable, TJsonVariable>(proj);

        auto actNameInput = TrecPointerKey::TrecFromSoft<>(nameInput);

        jProj->SetField(L"ResourceSource", TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(this->resourceSource));
        jProj->SetField(L"ResourceTitle", TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(this->resourceTitle));
        if(nameRequired)
            jProj->SetField(L"Name", TrecPointerKey::ConvertPointer<TStringVariable, TVariable>(actNameInput->GetTextElement()->GetText()));
        else
            jProj->SetField(L"ResourceDetail", TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(this->resourceDetails));


        dialogWindow->SetResult(proj);
    }
    Close();
}

void NewResourceHandler::OnUpdateName(TrecPointer<TPage> tc, EventArgs ea)
{
}

void NewResourceHandler::OnResourceSelect(TrecPointer<TPage> tc, EventArgs ea)
{
    UINT index = static_cast<UINT>(ea.arrayLabel);

    TrecPointer<TVariable> resourceType;

    if (this->resourceList.Get() && resourceList->At(resourceType, index) &&
        resourceType.Get() && resourceType->GetVarType() == var_type::list)
    {
        auto resourceEntry = TrecPointerKey::ConvertPointer<TVariable, TArrayVariable>(resourceType);

        for (UINT Rust = 0; resourceEntry->At(resourceType, Rust); Rust++)
        {
            if (!resourceType.Get()) continue;

            TString val(dynamic_cast<TStringVariable*>(resourceType->ToString().Get())->GetString());

            auto pieces = val.splitn(L":", 2);

            if (pieces->Size() != 2)continue;

            if (!pieces->at(0).CompareNoCase(L"Details"))
            {
                this->resourceDetails.Set(pieces->at(1).GetTrim());
                nameRequired = false;
            }
            else if (!pieces->at(0).CompareNoCase(L"Env-Source"))
            {
                this->resourceSource.Set(pieces->at(1).GetTrim());
            }
            else if (!pieces->at(0).CompareNoCase(L"Resource"))
            {
                this->resourceTitle.Set(pieces->at(1).GetTrim());
            }
            else if (!pieces->at(0).CompareNoCase(L"File-Ext"))
            {
                nameRequired = true;
            }
        }

        auto actNameInput = TrecPointerKey::TrecFromSoft<>(nameInput);
        actNameInput->setActive(nameRequired);
    }

}
