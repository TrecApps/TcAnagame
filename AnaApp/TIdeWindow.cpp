#include "TIdeWindow.h"
#include <AnafacePage.h>
#include <TLayout.h>
#include <TSwitchControl.h>
#include <TIdeLayout.h>
#include "PageTypes.h"
#include <TFormatReader.h>
#include "AnagameEnvironment.h"
#include <TPrimitiveVariable.h>
#include <TcOperator.h>

void TIdeWindow::SaveIde()
{
	if (!environment.Get())
	{
		TrecPointerKey::GetNewTrecPointer< BasicAnagameEnvironment>();
		return;
	}

	auto projEnv = environment->RetrieveProjectEnvironment();

	if (!projEnv.Get())return;

	projEnv->Save();

	auto directory = projEnv->GetDirectory();

	if (!directory.Get())
		return;

	TFile file(directory, L".tc_ide.json", TFile::t_file_create_always | TFile::t_file_truncate_existing | TFile::t_file_share_write);
	if (!file.IsOpen())
		return;
	directory = TFileShell::GetFileInfo(file.GetFilePath());
	file.Close();

	TrecPointer<TFormatReader> jsonWriter = TFormatReader::GetReader(directory);
	if (!jsonWriter.Get())
		return;
	// To-Do: Collect Variables to write
	jsonWriter->Write(TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TJsonVariable>());

}

void TIdeWindow::LoadLayout(const TString& layoutFile)
{
	auto anaMainPage = TrecPointerKey::ConvertPointer<TPage, TControl>(mainPage);
	if (!anaMainPage.Get())
		return;

	TrecPointer<TIdeLayout> ideLayout = TrecPointerKey::ConvertPointer<TControl, TIdeLayout>(anaMainPage->GetControlById(L"TC_IDE_LAYOUT"));
	if (!ideLayout.Get())
		return;

	TString path(GetDirectoryWithSlash(CentralDirectories::cd_Executable) + L"Layouts" + TC_FILE_SEP + layoutFile + L".layout.json");
	
	TrecPointer<TFormatReader> reader = TFormatReader::GetReader(TFileShell::GetFileInfo(path));

	if (!reader.Get())
		return;

	TString readResult(reader->Read());

	if (readResult.GetSize())return;

	ideLayout->SetUpLayout(TrecPointerKey::ConvertPointer<TVariable, TJsonVariable>(reader->GetData()));

}

void TIdeWindow::OnResize(int w, int h)
{
	// int curWidth = area.right - area.left;

	environment->SetProperty(L"window.ide.width", TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TPrimitiveVariable>(w), env_target::user);
	environment->SetProperty(L"window.ide.height", TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TPrimitiveVariable>(h), env_target::user);


	TWindow::OnResize(w, h);
}

void TIdeWindow::PrepResize()
{
	if (doResize)
	{
		glfwSetWindowSize(window, setTo.right, setTo.bottom);
		doResize = false;
		//OnResize(setTo.right, setTo.bottom);
	}
}

void TIdeWindow::SetFocusObject(TrecPointer<TObject> focusObject)
{
	TrecPointer<TPage> filePage = TrecPointerKey::ConvertPointer<TObject, TPage>(focusObject);
	if (filePage.Get())
		this->currentFilePage = TrecPointerKey::SoftFromTrec<>(filePage);
}

TIdeWindow::TIdeWindow(GLFWwindow* window): TWindow(window)
{
	mainPageSpace = 180;
	environment = TrecPointerKey::GetNewTrecPointer< BasicAnagameEnvironment>();


	auto height = environment->GetProperty(L"window.ide.height", env_target::user);
	auto width = environment->GetProperty(L"window.ide.width", env_target::user);

	doResize = false;

	setTo = { 0,0,0,0 };

	if (height.Get())
	{
		DoubleLong dl = DoubleLong::GetValueFromPrimitive(height);
		if (dl.type != double_long::dl_invalid)
		{
			doResize = true;
			switch (dl.type)
			{
			case double_long::dl_sign:
				setTo.bottom = dl.value.s;
				break;
			case double_long::dl_unsign:
				setTo.bottom = dl.value.u;
				break;
			case double_long::dl_double:
				setTo.bottom = dl.value.d;
			}
		}
	}

	if (width.Get())
	{
		DoubleLong dl = DoubleLong::GetValueFromPrimitive(width);
		if (dl.type != double_long::dl_invalid)
		{
			doResize = true;
			switch (dl.type)
			{
			case double_long::dl_sign:
				setTo.right = dl.value.s;
				break;
			case double_long::dl_unsign:
				setTo.right = dl.value.u;
				break;
			case double_long::dl_double:
				setTo.right = dl.value.d;
			}
		}
	}
}

void TIdeWindow::PrepResource(TrecPointer<TJsonVariable> resourceData)
{
	TString idePage, source, title, name;

	TrecPointer<TVariable> field;
	bool useResourceTitle = false;
	if (resourceData->RetrieveField(L"ResourceSource", field))
	{
		source.Set(TStringVariable::Extract(field, L""));
	}
	if (resourceData->RetrieveField(L"ResourceTitle", field))
	{
		title.Set(TStringVariable::Extract(field, L""));
	}	
	if (resourceData->RetrieveField(L"Name", field))
	{
		name.Set(TStringVariable::Extract(field, L""));
	}
	else if (resourceData->RetrieveField(L"ResourceDetail", field))
	{
		useResourceTitle = true;
		name.Set(TStringVariable::Extract(field, L""));
	}
	if (resourceData->RetrieveField(L"IdeTab", field))
	{
		idePage.Set(TStringVariable::Extract(field, L""));
	}


	TrecPointer<TObject> resource = environment->GetResource(source + L":" + title + L":" + name);

	TrecPointer<TPageBuilder> builder = TrecPointerKey::ConvertPointer<TObject, TPageBuilder>(resource);

	if (builder.Get())
	{
		builder->SetSpace(area);
		builder->SetDrawingBoard(TrecPointerKey::TrecFromSoft<>(this->self));
		TrecPointer<TPage> page = builder->GetPage();

		if (!page.Get())
			return;

		auto anaMainPage = TrecPointerKey::ConvertPointer<TPage, TControl>(mainPage);
		if (!anaMainPage.Get())
			return;

		TrecPointer<TIdeLayout> ideLayout = TrecPointerKey::ConvertPointer<TControl, TIdeLayout>(anaMainPage->GetControlById(L"TC_IDE_LAYOUT"));
		if (!ideLayout.Get())
			return;
		TrecPointer<TPage::EventHandler> newHandler = page->GetHandler();

		if (dynamic_cast<TFileEventHandler*>(newHandler.Get()))
		{
			// We have a file Page at play. Wrap it around with a File Page
			page = TrecPointerKey::GetNewSelfTrecPointerAlt<TPage, TFilePage>(TrecActivePointer<TPage>(page), name, L"");

			// To-Do: Means of IDE window being able to keep Track of the file Pages
			this->registeredPages.push_back(TrecPointerKey::SoftFromTrec<>(page));
		}

		ideLayout->SubmitToTab(idePage, useResourceTitle ? title : name, page);
		needsRefresh = true;
	}
}

void TIdeWindow::SetMainPage(TrecPointer<TPage> mainPage)
{
	if (!environment.Get())
	{
		environment = TrecPointerKey::GetNewTrecPointer< BasicAnagameEnvironment>();
	}
	this->mainPage.Nullify();

	this->mainPage = TrecPointerKey::GetNewSelfTrecPointerAlt<TPage, TLayout>(TWindow::GetDrawingBoard(), TDataMap<TDataMap<TString>>());
	TrecPointer<TLayout> mainLayout = TrecPointerKey::ConvertPointer<TPage, TLayout>(this->mainPage);

	mainLayout->AddAttribute(L"IsStack", L"true");
	mainLayout->AddRow(mainPageSpace, false);
	mainLayout->AddRow(1, true);

	TrecPointer<TPage> page = TrecPointerKey::GetNewSelfTrecPointerAlt<TPage, TSwitchControl>(TWindow::GetDrawingBoard(), TDataMap<TDataMap<TString>>());
	ribbonTabs = TrecPointerKey::ConvertPointer<TPage, TSwitchControl>(page);
	mainLayout->AddPage(page, 0, 0);

	page = TrecPointerKey::GetNewSelfTrecPointerAlt<TPage, TIdeLayout>(TWindow::GetDrawingBoard(), TDataMap<TDataMap<TString>>());
	TrecPointer<TIdeLayout> ideLayout = TrecPointerKey::ConvertPointer<TPage, TIdeLayout>(page);
	ideLayout->AddAttribute(L"ID", L"TC_IDE_LAYOUT");
	mainLayout->AddPage(page, 1, 0);

	// Now Set up The Main Page Provided
	TrecPointer<TRibbonPage> ribbonPage = TrecPointerKey::ConvertPointer<TPage, TRibbonPage>(mainPage);
	if (ribbonPage.Get() && ribbonPage->GetPageType() == page_type::main_ribbon)
	{
		ribbonTabs->AddPage(mainPage, 0, 0, true);
	}

	mainLayout->onCreate(this->area, TrecPointer<TFileShell>());
	TDataArray<TPage::EventID_Cred> cred;
	mainLayout->OnResize(this->area, 0, cred);

	TString layout(TStringVariable::Extract(environment->TEnvironment::GetProperty(L"anagame.ide.layout"), L"code"));
	LoadLayout(layout);

}

bool TIdeWindow::PrepProject(const TProjectData& projectData)
{
	auto instance = TInstance::GetInstance();
	//TDataArray<TString> lib;
	//lib.push_back(projectData.builderName);
	//auto failed = instance->LoadLibraries(lib);
	//if (failed.Size() == 0)
	//	return false;

	auto env = instance->GetEnvironment(projectData);
	TrecPointer<AGProjectEnvironment> projEnv = 
		TrecPointerKey::ConvertPointer<TEnvironment, AGProjectEnvironment>(env);

	if (!projEnv.Get())
		return false;

	instance->SaveProject(projectData);

	return SetProject(TrecActivePointer<AGProjectEnvironment>(projEnv));
}

bool TIdeWindow::SetProject(TrecActivePointer<AGProjectEnvironment> project)
{
	auto curProj = environment->RetrieveProjectEnvironment();
	if (curProj.Get())
	{
		curProj->Save();
	}

	this->environment->SetProject(project);
	//SetMainPage(TrecPointer<TPage>());

	auto actProject = project.GetTrecPointer();

	TrecPointer<TFileShell> fileShell = actProject->GetDirectory();
	if (!fileShell.Get())return false;

	TString path(fileShell->GetPath() + L"/.tc_ide.json");
	fileShell = TFileShell::GetFileInfo(path);

	if (!fileShell.Get())
	{
		SaveIde();
		actProject->Refresh();
		return true;
	}
	TrecPointer<TFormatReader> formatReader = TFormatReader::GetReader(fileShell);

	if (!formatReader.Get())
		return false;

	formatReader->Read();
	auto vars = formatReader->GetData();

	assert(vars.Get() && vars->GetVarType() == var_type::json);

	TrecPointer<TJsonVariable> rootVar = TrecPointerKey::ConvertPointer<TVariable, TJsonVariable>(vars);

	auto mainLayout = TrecPointerKey::ConvertPointer<TPage, TLayout>(mainPage);
	TrecPointer<TIdeLayout> ideLayout = TrecPointerKey::ConvertPointer<TPage, TIdeLayout>(mainLayout->GetPage(1, 0));

	TrecPointer<TVariable> ideLayoutVar;
	if (rootVar->RetrieveField(L"", ideLayoutVar))
	{
		auto jsonLayout = TrecPointerKey::ConvertPointer<TVariable, TJsonVariable>(ideLayoutVar);
		if(jsonLayout.Get())
			ideLayout->SetUpLayout(jsonLayout);
	}
	actProject->Refresh();
	return true;
}

void TIdeWindow::SetIdeProperty(ide_property ideProp, const TString& prop)
{
	switch (ideProp)
	{
	case ide_property::drag_management:
		dragManager = TrecPointerKey::ConvertPointer<TObject, TPage>(environment->GetResource(prop));
		break;
	case ide_property::main_page_space:
		if (!TString::ConvertStringToUint(prop, mainPageSpace))
			mainPageSpace = 100;
	}
}

void TIdeWindow::ActivateRibbons(const TDataArray<TString>& names)
{
	if (!ribbonTabs.Get())
		return;

	TrecPointer<TabBar> tabBar = ribbonTabs->GetTabBar();

	for (UINT Rust = 0; Rust < tabBar->GetTabCount(); Rust++)
	{
		TrecPointer<TPage> tab = tabBar->GetTabAt(Rust);
		TrecPointer<TPage> tabContent = tabBar->GetTabAt(Rust, true);

		TrecPointer<TPageEx> tabContentEx = TrecPointerKey::ConvertPointer<TPage, TPageEx>(tabContent);
		TrecPointer<TabBar::Tab> actTab = TrecPointerKey::ConvertPointer<TPage, TabBar::Tab>(tab);

		// This is a main ribbon, we are not deactivating it
		if (tabContentEx.Get() && tabContentEx->GetPageType() == page_type::main_ribbon)
			continue;

		assert(actTab.Get());
		TString tabName(actTab->GetName());
		bool found = false;

		for (UINT C = 0; !found && C < names.Size(); C++)
		{
			if (!tabName.Compare(names[Rust]))
				found = true;
		}

		tabBar->ActivateTab(tab, found);
	}
}

UINT TIdeWindow::AddRibbon(TrecActivePointer<TPageEx> ribbon)
{
	if (!ribbonTabs.Get())
		return 1;

	TrecPointer<TPageEx> baseRibbon = ribbon.GetTrecPointer();

	if(baseRibbon->GetPageType() == page_type::file || baseRibbon->GetPageType() == page_type::file_sup)
		return 2;

	TString ribbonName(baseRibbon->GetName());

	auto tabs = ribbonTabs->GetTabBar();
	assert(tabs.Get());

	for (UINT Rust = 0; Rust < tabs->GetTabCount(); Rust++)
	{
		TrecPointer<TabBar::Tab> tab = TrecPointerKey::ConvertPointer<TPage, TabBar::Tab>(tabs->GetTabAt(Rust));
		if (!ribbonName.Compare(tab->GetName()))
			return 3;
	}

	tabs->AddNewTab(ribbonName, TrecPointerKey::ConvertPointer<TPageEx, TPage>(baseRibbon), false);
	return 0;
}

void TIdeWindow::RemoveRibbon(const TString& ribbonName)
{
	if (!ribbonTabs.Get()) return;

	auto tabs = ribbonTabs->GetTabBar();
	assert(tabs.Get());

	for (UINT Rust = 0; Rust < tabs->GetTabCount(); Rust++)
	{
		auto pTab = tabs->GetTabAt(Rust);
		TrecPointer<TabBar::Tab> tab = TrecPointerKey::ConvertPointer<TPage, TabBar::Tab>(pTab);
		if (!ribbonName.Compare(tab->GetName()))
		{
			tabs->RemoveTab(pTab);
			return;
		}
	}
}


TrecPointer<BasicAnagameEnvironment> TIdeWindow::GetEnvironment()
{
	return environment;
}

void TIdeWindow::SaveCurrent()
{
	TrecPointer<TFilePage> filePage = TrecPointerKey::ConvertPointer<TPage, TFilePage>(
		TrecPointerKey::TrecFromSoft<>(this->currentFilePage)
	);
	TString res;
	if (filePage.Get())
		filePage->SaveFile(res);
}

void TIdeWindow::SaveAll()
{
	for (UINT Rust = 0; Rust < this->registeredPages.Size(); Rust++)
	{
		TString res;
		TrecPointer<TFilePage> filePage = TrecPointerKey::ConvertPointer<TPage, TFilePage>(
			TrecPointerKey::TrecFromSoft<>(this->registeredPages[Rust])
		);

		if (filePage.Get())
			filePage->SaveFile(res);
	}
}
