#include "MainPageIdeHandler.h"
#include <TDialog.h>
#include <AnagameEnvironment.h>
#include <TContainerVariable.h>
#include "ProjectDialogHandler.h"
#include "NewResourceHandler.h"

// Found on the Home Tab
TString on_LoadNewSolution(L"LoadNewSolution");
TString on_SaveFile(L"OnSave");
TString on_SaveAll(L"OnSaveAll");
TString on_NewResource(L"OnNewResource");
TString on_ImportFile(L"OnImportFile");
TString on_Print(L"OnPrint");


MainPageIdeHandler::MainPageIdeHandler()
{
	events.addEntry(on_LoadNewSolution, 0);
	calls.push_back(&MainPageIdeHandler::OnLoadNewSolution);

	events.addEntry(on_SaveFile, 1);
	calls.push_back(&MainPageIdeHandler::OnSaveFile);

	events.addEntry(on_SaveAll, 2);
	calls.push_back(&MainPageIdeHandler::OnSaveAllFiles);

	events.addEntry(on_NewResource, 3);
	calls.push_back(&MainPageIdeHandler::OnNewResource);

	events.addEntry(on_ImportFile, 4);
	calls.push_back(&MainPageIdeHandler::OnImportFile);

	events.addEntry(on_Print, 5);
	calls.push_back(&MainPageIdeHandler::OnPrint);
}

void MainPageIdeHandler::Initialize(TrecPointer<TPage> page)
{
	TrecPointer<TIdeWindow> ideWindow = TrecPointerKey::ConvertPointer<DrawingBoard, TIdeWindow>(page->GetDrawingBoard());
	window = TrecPointerKey::SoftFromTrec<TIdeWindow>(ideWindow);

	TrecPointer<AnafacePage> anaPage = TrecPointerKey::ConvertPointer<TPage, AnafacePage>(page);
	auto column = anaPage->GetControlById(L"ResourceColumn");
	column->setActive(false);
	this->resourceColumn = TrecPointerKey::SoftFromTrec<>(column);
	auto textProj = anaPage->GetControlById(L"ProjectNameBox");
	this->projectName = TrecPointerKey::SoftFromTrec<>(textProj);
}

bool MainPageIdeHandler::OnChar(bool fromChar, UINT nChar, UINT nRepCnt, UINT nFlags, message_output* mOut)
{
	return false;
}

void MainPageIdeHandler::SetSelf(TrecPointer<TPage::EventHandler> handleSelf)
{
}

TrecPointer<TPage> MainPageIdeHandler::GetPage()
{
	return TrecPointer<TPage>();
}

void MainPageIdeHandler::HandleEvents(TDataArray<TPage::EventID_Cred>& eventAr)
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

void MainPageIdeHandler::OnLoadNewSolution(TrecPointer<TPage> tc, EventArgs ea)
{
	TrecPointer<TWindow> baseDialog;
	TrecPointer<TInstance> instance = TInstance::GetInstance();
	auto actWindow = TrecPointerKey::ConvertPointer<TIdeWindow, TWindow>(TrecPointerKey::TrecFromSoft<>(window));

	auto pageBuilder = TrecPointerKey::GetNewTrecPointerAlt<TPageBuilder, AnafaceBuilder>();
	pageBuilder->SetHandler(TrecPointerKey::GetNewSelfTrecPointerAlt<TPage::EventHandler, ProjectDialogHandler>());

	TDataArray<TString> dat;
	//instance->LoadLibraries(dat);

	instance->GenerateDialog(baseDialog,										// the dialog we're expecting
		actWindow,																// The IDE Window Serving as a parent
		L"Load Project",														// The Title of the dialog
		pageBuilder,	// Builder for the Page within
		TFileShell::GetFileInfo(GetDirectoryWithSlash(							// File to load the UI from
			CentralDirectories::cd_Executable) + L"UI/ProjectDialog.json"),
		L"",																	// Not needed, alternative to previous param
		t_dialog_modal_mode::soft_modal);										// Move parent window around but otherwise DNI

	TrecPointer<TDialog> dialog = TrecPointerKey::ConvertPointer<TWindow, TDialog>(baseDialog);
	assert(dialog.Get());

	dialog->RunModal();

	TrecPointer<TJsonVariable> data = TrecPointerKey::ConvertPointer<TVariable, TJsonVariable>(dialog->GetResult());
	if (data.Get())
	{
		// User Pressed Okay
		TProjectData pData;
		assert(pData.Initialize(data));

		auto ideWindow = TrecPointerKey::ConvertPointer<TWindow, TIdeWindow>(actWindow);
		if (ideWindow->PrepProject(pData))
		{
			auto column = TrecPointerKey::TrecFromSoft<>(resourceColumn);
			if (column.Get())
				column->setActive(true);
			auto textProj = TrecPointerKey::ConvertPointer<TControl, TTextInput>(TrecPointerKey::TrecFromSoft<>(this->projectName));
			TrecPointer<TVariable> projName;
			if (textProj.Get() && data->RetrieveField(L"name", projName))
				textProj->SetText(TStringVariable::Extract(projName, L""));
		}
		actWindow->PrepRefresh();
	}
}

void MainPageIdeHandler::OnSaveFile(TrecPointer<TPage> tc, EventArgs ea)
{
	auto actWindow = TrecPointerKey::ConvertPointer<TIdeWindow, TWindow>(TrecPointerKey::TrecFromSoft<>(window));
	auto ideWindow = TrecPointerKey::ConvertPointer<TWindow, TIdeWindow>(actWindow);

	ideWindow->SaveCurrent();
}

void MainPageIdeHandler::OnSaveAllFiles(TrecPointer<TPage> tc, EventArgs ea)
{
	auto actWindow = TrecPointerKey::ConvertPointer<TIdeWindow, TWindow>(TrecPointerKey::TrecFromSoft<>(window));
	auto ideWindow = TrecPointerKey::ConvertPointer<TWindow, TIdeWindow>(actWindow);

	ideWindow->SaveAll();
}

void MainPageIdeHandler::OnNewResource(TrecPointer<TPage> tc, EventArgs ea)
{
	TrecPointer<TWindow> baseDialog;
	TrecPointer<TInstance> instance = TInstance::GetInstance();
	auto actWindow = TrecPointerKey::ConvertPointer<TIdeWindow, TWindow>(TrecPointerKey::TrecFromSoft<>(window));

	auto pageBuilder = TrecPointerKey::GetNewTrecPointerAlt<TPageBuilder, AnafaceBuilder>();
	pageBuilder->SetHandler(TrecPointerKey::GetNewSelfTrecPointerAlt<TPage::EventHandler, NewResourceHandler>());

	instance->GenerateDialog(baseDialog,										// the dialog we're expecting
		actWindow,																// The IDE Window Serving as a parent
		L"Create Project Resource",														// The Title of the dialog
		pageBuilder,	// Builder for the Page within
		TFileShell::GetFileInfo(GetDirectoryWithSlash(							// File to load the UI from
			CentralDirectories::cd_Executable) + L"UI/ResourceDialog.json"),
		L"",																	// Not needed, alternative to previous param
		t_dialog_modal_mode::soft_modal);										// Move parent window around but otherwise DNI

	TrecPointer<TDialog> dialog = TrecPointerKey::ConvertPointer<TWindow, TDialog>(baseDialog);
	assert(dialog.Get());

	dialog->RunModal();

	TrecPointer<TJsonVariable> data = TrecPointerKey::ConvertPointer<TVariable, TJsonVariable>(dialog->GetResult());
	if (data.Get())
		TrecPointerKey::TrecFromSoft<>(window)->PrepResource(data);
}

void MainPageIdeHandler::OnImportFile(TrecPointer<TPage> tc, EventArgs ea)
{
}

void MainPageIdeHandler::OnPrint(TrecPointer<TPage> tc, EventArgs ea)
{
}
