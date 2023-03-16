#include "TIdeWindow.h"
#include <AnafacePage.h>
#include <TLayout.h>
#include <TSwitchControl.h>
#include <TIdeLayout.h>
#include "PageTypes.h"
#include <TFormatReader.h>
#include "AnagameEnvironment.h"

void TIdeWindow::SaveIde()
{
	if (!environment.Get())
	{
		TrecPointerKey::GetNewTrecPointer< BasicAnagameEnvironment>();
		return;
	}

	auto projEnv = environment->RetrieveProjectEnvironment();

	if (!projEnv.Get())return;
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

void TIdeWindow::OnResize(int w, int h)
{
	// int curWidth = area.right - area.left;
	TWindow::OnResize(w, h);
}

TIdeWindow::TIdeWindow(GLFWwindow* window): TWindow(window)
{
	mainPageSpace = 180;
}

void TIdeWindow::SetMainPage(TrecPointer<TPage> mainPage)
{
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

	TrecPointer<IdeSection> ideSection;
	assert(ideLayout->InitDivide(true, false, 0.75f));
	page = TrecPointerKey::GetNewSelfTrecPointerAlt<TPage, TSwitchControl>(TWindow::GetDrawingBoard(), TDataMap<TDataMap<TString>>());
	dynamic_cast<TSwitchControl*>(page.Get())->onCreate(area, TrecPointer<TFileShell>());
	
	ideSection = ideLayout->GetRootSection();

	
	assert(ideLayout->DivideSection(ideSection, false, false, 0.66f));

	assert(ideLayout->AppendSection(ideSection, TrecPointerKey::ConvertPointer<TPage, TSwitchControl>(page)));
	ideSection = ideLayout->GetFirstSection(ideSection);

	page = TrecPointerKey::GetNewSelfTrecPointerAlt<TPage, TSwitchControl>(TWindow::GetDrawingBoard(), TDataMap<TDataMap<TString>>());
	dynamic_cast<TSwitchControl*>(page.Get())->onCreate(area, TrecPointer<TFileShell>());

	assert(ideLayout->AppendSection(ideSection, TrecPointerKey::ConvertPointer<TPage, TSwitchControl>(page)));

	page = TrecPointerKey::GetNewSelfTrecPointerAlt<TPage, TSwitchControl>(TWindow::GetDrawingBoard(), TDataMap<TDataMap<TString>>());
	dynamic_cast<TSwitchControl*>(page.Get())->onCreate(area, TrecPointer<TFileShell>());

	assert(ideLayout->AppendSection(ideSection, TrecPointerKey::ConvertPointer<TPage, TSwitchControl>(page)));

	if (!environment.Get())
	{
		environment = TrecPointerKey::GetNewTrecPointer< BasicAnagameEnvironment>();
	}
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

	return SetProject(TrecActivePointer<AGProjectEnvironment>(projEnv));
}

bool TIdeWindow::SetProject(TrecActivePointer<AGProjectEnvironment> project)
{
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
