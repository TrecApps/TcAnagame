#include "ProjectDialogHandler.h"
#include <TContainerVariable.h>
#include <TDataLayout.h>

TString onDirectorySelect(L"OnDirectorySelect");
TString onOkay(L"OnOkay");
TString onCancel(L"OnCancel");

TString onUpdateName(L"OnUpdateName");
TString onSelectRecent(L"OnSelectRecent");
TString onSelectNew(L"OnSelectNew");


ProjectDialogHandler::ProjectDialogHandler()
{
    calls.push_back(&ProjectDialogHandler::OnDirectorySelect);
    events.addEntry(onDirectorySelect, 0);

    calls.push_back(&ProjectDialogHandler::OnOkay);
    events.addEntry(onOkay, 1);

    calls.push_back(&ProjectDialogHandler::OnCancel);
    events.addEntry(onCancel, 2);

    calls.push_back(&ProjectDialogHandler::OnUpdateName);
    events.addEntry(onUpdateName, 3);

    calls.push_back(&ProjectDialogHandler::OnSelectNew);
    events.addEntry(onSelectNew, 4);

    calls.push_back(&ProjectDialogHandler::OnSelectRecent);
    events.addEntry(onSelectRecent, 5);
}

void ProjectDialogHandler::Initialize(TrecPointer<TPage> page)
{
    dialogWindow = TrecPointerKey::ConvertPointer<DrawingBoard, TDialog>(page->GetDrawingBoard());

    TrecPointer<AnafacePage> aPage = TrecPointerKey::ConvertPointer<TPage, AnafacePage>(page);

    auto control = aPage->GetControlById(L"OkButton");
    okayButton = TrecPointerKey::SoftFromTrec<>( control );

    auto textControl = TrecPointerKey::ConvertPointer<TControl, TTextInput>(aPage->GetControlById(L"DirectoryField"));
    inputDirectory = TrecPointerKey::SoftFromTrec<>(textControl);
    textControl = TrecPointerKey::ConvertPointer<TControl, TTextInput>(aPage->GetControlById(L"NameField"));
    inputName = TrecPointerKey::SoftFromTrec<>(textControl);

    auto instance = TInstance::GetInstance();

    existingProjects = TrecPointerKey::ConvertPointer<TVariable, TArrayVariable>(instance->GetExisitngProjects());
    availableProjects = TrecPointerKey::ConvertPointer<TVariable, TArrayVariable>(instance->GetAvailableProjectTypes());

    auto dataList = TrecPointerKey::ConvertPointer<TControl, TDataLayout>(aPage->GetControlById(L"NewProjectList"));
    dataList->SetData(TrecPointerKey::ConvertPointer<TArrayVariable, TVariable>(availableProjects));

    dataList = TrecPointerKey::ConvertPointer<TControl, TDataLayout>(aPage->GetControlById(L"RecentProjectList"));
    dataList->SetData(TrecPointerKey::ConvertPointer<TArrayVariable, TVariable>(existingProjects));

    AssessOkay();
}

bool ProjectDialogHandler::OnChar(bool fromChar, UINT nChar, UINT nRepCnt, UINT nFlags, message_output* mOut)
{
    return false;
}

void ProjectDialogHandler::SetSelf(TrecPointer<TPage::EventHandler> handleSelf)
{
}

TrecPointer<TPage> ProjectDialogHandler::GetPage()
{
    return TrecPointer<TPage>();
}

void ProjectDialogHandler::HandleEvents(TDataArray<TPage::EventID_Cred>& eventAr)
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
            // call method
            if (calls[e_id])
                (this->*calls[e_id])(eventAr[c].control, ea);
        }
        eventAr[c].args.Nullify();
    }
}

void ProjectDialogHandler::Close()
{
    inputDirectory.Nullify();
    inputName.Nullify();
    auto win = dialogWindow;
    dialogWindow.Nullify();
    if (win.Get())
    {
        win->Close();
        
    }
}

void ProjectDialogHandler::AssessOkay()
{
    bool works = true;
    if (!projectData.builderName.GetTrim().GetSize())
        works = false;
    if (works && !projectData.environmentName.GetTrim().GetSize())
        works = false;
    if (works && !projectData.projectName.GetTrim().GetSize())
        works = false;
    if (works && !projectData.directory.Get())
        works = false;

    auto oButton = TrecPointerKey::TrecFromSoft<>(okayButton);
    oButton->setActive(works);

    dialogWindow->PrepRefresh();
}

void ProjectDialogHandler::OnDirectorySelect(TrecPointer<TPage> tc, EventArgs ea)
{
    TString error;
    TrecPointer<TFileShell> targetDirectory = OpenDirectoryDialog(TrecPointer<TDirectory>(), error);
    TrecPointer<TDirectory> actDirectory = TrecPointerKey::ConvertPointer<TFileShell, TDirectory>(targetDirectory);
    if (actDirectory.Get())
    {
        this->projectData.directory = actDirectory;
        TrecPointerKey::TrecFromSoft<>(this->inputDirectory)->SetText(actDirectory->GetPath());
    }
    AssessOkay();
}

void ProjectDialogHandler::OnCancel(TrecPointer<TPage> tc, EventArgs ea)
{
    TInstance::GetInstance()->OnWindowClose(dialogWindow->GetGlfwWindow());
    glfwDestroyWindow(dialogWindow->GetGlfwWindow());
}

void ProjectDialogHandler::OnOkay(TrecPointer<TPage> tc, EventArgs ea)
{
    if (dialogWindow.Get())
    {
        TrecPointer<TVariable> proj = TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TJsonVariable>();
        TrecPointer<TJsonVariable> jProj = TrecPointerKey::ConvertPointer<TVariable, TJsonVariable>(proj);

        jProj->SetField(L"builderType", TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(projectData.builderName));
        jProj->SetField(L"projectType", TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(projectData.environmentName));
        jProj->SetField(L"name", TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(projectData.projectName));
        jProj->SetField(L"directory", TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(projectData.directory->GetPath()));


        dialogWindow->SetResult(proj);
    }
    Close();
}

void ProjectDialogHandler::OnSelectNew(TrecPointer<TPage> tc, EventArgs ea)
{
    UINT index = static_cast<UINT>(ea.arrayLabel);

    TrecPointer<TVariable> proj;
    if (this->availableProjects.Get() && availableProjects->At(proj, index))
    {
        auto jProj = TrecPointerKey::ConvertPointer<TVariable, TJsonVariable>(proj);
        assert(jProj.Get());

        if (jProj->RetrieveField(L"builderType", proj))
        {
            auto strVar = TrecPointerKey::ConvertPointer<TVariable, TStringVariable>(proj);
            assert(strVar.Get());
            projectData.builderName.Set(strVar->GetString());

        }

        if (jProj->RetrieveField(L"projectType", proj))
        {
            auto strVar = TrecPointerKey::ConvertPointer<TVariable, TStringVariable>(proj);
            assert(strVar.Get());
            projectData.environmentName.Set(strVar->GetString());
        }

    }
    AssessOkay();
}

void ProjectDialogHandler::OnSelectRecent(TrecPointer<TPage> tc, EventArgs ea)
{
    UINT index = static_cast<UINT>(ea.arrayLabel);

    TrecPointer<TVariable> proj;
    if (this->existingProjects.Get() && existingProjects->At(proj, index))
    {
        auto jProj = TrecPointerKey::ConvertPointer<TVariable, TJsonVariable>(proj);
        assert(jProj.Get());

        if (jProj->RetrieveField(L"builderType", proj))
        {
            auto strVar = TrecPointerKey::ConvertPointer<TVariable, TStringVariable>(proj);
            assert(strVar.Get());
            projectData.builderName.Set(strVar->GetString());
        }

        if (jProj->RetrieveField(L"projectType", proj))
        {
            auto strVar = TrecPointerKey::ConvertPointer<TVariable, TStringVariable>(proj);
            assert(strVar.Get());
            projectData.environmentName.Set(strVar->GetString());
        }

        if (jProj->RetrieveField(L"name", proj))
        {
            auto strVar = TrecPointerKey::ConvertPointer<TVariable, TStringVariable>(proj);
            assert(strVar.Get());
            projectData.projectName.Set(strVar->GetString());
            auto iButton = TrecPointerKey::TrecFromSoft<>(inputName);
            iButton->SetText(projectData.projectName);
        }

        if (jProj->RetrieveField(L"directory", proj))
        {
            auto strVar = TrecPointerKey::ConvertPointer<TVariable, TStringVariable>(proj);
            assert(strVar.Get());
            projectData.directory = TrecPointerKey::ConvertPointer<TFileShell,TDirectory>(TFileShell::GetFileInfo(strVar->GetString()));
            auto iDirectory = TrecPointerKey::TrecFromSoft<>(inputDirectory);
            if (projectData.directory.Get())
                iDirectory->SetText(projectData.directory->GetPath());
        }
    }
    AssessOkay();
}

void ProjectDialogHandler::OnUpdateName(TrecPointer<TPage> tc, EventArgs ea)
{
    projectData.projectName.Set(ea.text);
    AssessOkay();
}
