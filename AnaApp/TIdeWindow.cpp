#include "TIdeWindow.h"
#include <AnafacePage.h>
#include <TLayout.h>
#include <TSwitchControl.h>
#include <TIdeLayout.h>
#include "PageTypes.h"

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
}

void TIdeWindow::SetProject(TrecActivePointer<AGProjectEnvironment> project)
{
	this->environment->SetProject(project);
	SetMainPage(TrecPointer<TPage>());
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
