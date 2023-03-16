#include <include/GL/glew.h>
#include <TInstance.h>
#include "../AnaWidget/TControl.h"
#include <AnagameEnvironment.h>
#include "MainPageIdeHandler.h"
#include <TAnagameCodeExEnv.h>

TrecPointer<TInstance> mainInstance;

void onChar(GLFWwindow* win, UINT ch);
void onMouseMove(GLFWwindow* window, double xpos, double ypos);
void onMouseClick(GLFWwindow* window, int button, int action, int mods);
void onScroll(GLFWwindow* window, double xoffset, double yoffset);
void onFocus(GLFWwindow* window, int focused);
void onWindowResize(GLFWwindow* window, int width, int height);
void onWindowClose(GLFWwindow* window);
void onKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods);
void onFrameResize(GLFWwindow* window, int width, int height);

int main()
{

	mainInstance = TInstance::GetInstance();

	//mainInstance->LoadLibraries(TDataArray<TString>());

	if (!mainInstance->GetGlfwInitResult())
		return -1;

	mainInstance->SetCallbacks(onChar, onMouseMove, onKeyPress, onMouseClick, onScroll, onFocus, onWindowResize, onWindowClose, onFrameResize);
	mainInstance->SubmitEnvironmentBuilder(TrecPointerKey::GetNewTrecPointerAlt<TEnvironmentBuilder, TAnagameCodeExBuilder>());
	
	TrecPointer<TWindow> mainWindow;

	mainInstance->GenerateWindow(mainWindow, TrecPointer<TFileShell>(), L"", t_window_type::t_window_type_ide);

	if (!mainWindow.Get())
	{
		mainInstance.Nullify();
		return -2;
	}
	glewExperimental = GL_TRUE;
	auto initRes = glewInit();
	auto chRes = glewGetErrorString(initRes);

	TDataMap<TDataMap<TString>> styles;

	//TrecPointer<TPage> page = TrecPointerKey::GetNewSelfTrecPointerAlt<TPage, TControl>(TrecPointerKey::ConvertPointer<TWindow, DrawingBoard>(mainWindow), styles);

	//TrecPointer<TControl> control = TrecPointerKey::ConvertPointer<TPage, TControl>(page);
	//control->AddAttribute(L"Margin", L"50,50,80,80");
	//control->AddAttribute(L"BorderThickness", L"3.0");
	//control->AddAttribute(L"BorderColor", L"0.1,1.0,0.1,1.0");

	//control->AddAttribute(L"ContentColor", L"0.2,0.5,0.2,1.0");

	//control->AddAttribute(L"Caption", L"DirectWrite");
	//control->AddAttribute(L"FontColor", L"0.9,0.3,0.2,0.7");
	//control->AddAttribute(L"FontSize", L"48.0");

	//control->onCreate(mainWindow->GetArea(), TrecPointer<TFileShell>());

	TrecPointer<AnafaceBuilder> builder = TrecPointerKey::GetNewTrecPointer<AnafaceBuilder>();
	builder->SetDrawingBoard(TrecPointerKey::ConvertPointer<TWindow, DrawingBoard>(mainWindow));
	builder->SetSpace(mainWindow->GetArea());
	builder->SetHandler(TrecPointerKey::GetNewSelfTrecPointerAlt<TPage::EventHandler, MainPageIdeHandler>());


	TString tmlFile(GetDirectoryWithSlash(CentralDirectories::cd_Executable));
	tmlFile.Append(L"UI\\MainIdeRibbon.json");
	TrecPointer<TFileShell> jsonFile = TFileShell::GetFileInfo(tmlFile);


	TrecPointer<TPage> ribbon = builder->GetPage(jsonFile);

	TrecActivePointer<TPage> activePage(ribbon);

	TrecPointer<TPage> mainRibbon = TrecPointerKey::GetNewSelfTrecPointerAlt<TPage, TRibbonPage>(activePage, L"AnagameBuilder", true);

	if (jsonFile.Get())
		mainWindow->SetMainPage(mainRibbon);

	


	while (mainInstance->HasWindows())
	{
		mainInstance->DoDraw();
		glfwPollEvents();
	}



	mainInstance.Nullify();

	return 0;
}

void onChar(GLFWwindow* win, UINT ch)
{
	mainInstance->OnChar(win, ch);
}

void onMouseMove(GLFWwindow* window, double xpos, double ypos)
{
	mainInstance->OnMouseMove(window, xpos, ypos);
}

void onMouseClick(GLFWwindow* window, int button, int action, int mods)
{
	switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		action ? mainInstance->OnLButtonDown(window, mods): mainInstance->OnLButtonUp(window,mods);
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		action ? mainInstance->OnRButtonDown(window, mods) : mainInstance->OnRButtonUp(window, mods);
	}
}

void onScroll(GLFWwindow* window, double xoffset, double yoffset)
{
	mainInstance->OnScroll(window, xoffset, yoffset);
}

void onFocus(GLFWwindow* window, int focused)
{
	if (focused)
		mainInstance->OnFocus(window);
	else
		mainInstance->OnLoseFocus(window);
}

void onWindowResize(GLFWwindow* window, int width, int height)
{
	mainInstance->OnWindowResize(window, width, height);
}

void onWindowClose(GLFWwindow* window)
{
	mainInstance->OnWindowClose(window);
}

void onKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	mainInstance->OnKeyPress(window, key, scancode, action, mods);
}

void onFrameResize(GLFWwindow* window, int width, int height)
{
	mainInstance->OnWindowBufferResize(window, width, height);
}