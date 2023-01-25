#include "MainPageIdeHandler.h"
#include <TDialog.h>
#include <AnagameEnvironment.h>
#include <TContainerVariable.h>

// Found on the Home Tab
TString on_LoadNewSolution(L"LoadNewSolution");
TString on_SaveFile(L"SaveFile");
TString on_SaveAll(L"SaveAllFiles");
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

	instance->GenerateDialog(baseDialog,										// the dialog we're expecting
		actWindow,																// The IDE Window Serving as a parent
		L"Load Project",														// The Title of the dialog
		TrecPointerKey::GetNewTrecPointerAlt<TPageBuilder, AnafaceBuilder>(),	// Builder for the Page within
		TFileShell::GetFileInfo(GetDirectoryWithSlash(							// File to load the UI from
			CentralDirectories::cd_Executable) + L"Resources/UI/ProjectDialog.json"),
		L"",																	// Not needed, alternative to previous param
		t_dialog_modal_mode::soft_modal);										// Move parent window around but otherwise DNI

	TrecPointer<TDialog> dialog = TrecPointerKey::ConvertPointer<TWindow, TDialog>(baseDialog);
	assert(dialog.Get());

	dialog->RunModal();

	TrecPointer<TJsonVariable> data = TrecPointerKey::ConvertPointer<TVariable, TJsonVariable>(dialog->GetResult());
	if (data.Get())
	{
		// User Pressed Okay
	}
}

void MainPageIdeHandler::OnSaveFile(TrecPointer<TPage> tc, EventArgs ea)
{
}

void MainPageIdeHandler::OnSaveAllFiles(TrecPointer<TPage> tc, EventArgs ea)
{
}

void MainPageIdeHandler::OnNewResource(TrecPointer<TPage> tc, EventArgs ea)
{
}

void MainPageIdeHandler::OnImportFile(TrecPointer<TPage> tc, EventArgs ea)
{
}

void MainPageIdeHandler::OnPrint(TrecPointer<TPage> tc, EventArgs ea)
{
}
