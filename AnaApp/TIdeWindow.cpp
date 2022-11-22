#include "TIdeWindow.h"
#include <AnafacePage.h>
#include <TLayout.h>
#include <TSwitchControl.h>
#include <TIdeLayout.h>

TIdeWindow::TIdeWindow(GLFWwindow* window): TWindow(window)
{
	mainPageSpace = 100;
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
	mainLayout->AddPage(page, 0, 0);

	page = TrecPointerKey::GetNewSelfTrecPointerAlt<TPage, TIdeLayout>(TWindow::GetDrawingBoard(), TDataMap<TDataMap<TString>>());
	TrecPointer<TIdeLayout> ideLayout = TrecPointerKey::ConvertPointer<TPage, TIdeLayout>(page);
	mainLayout->AddPage(page, 1, 0);

	mainLayout->onCreate(this->area, TrecPointer<TFileShell>());


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
