#include <GLFW/glfw3.h>
#include "TInstance.h"

TInstance::TInstance()
{
	glfwInitResult = glfwInit();

	charFunction = nullptr;
	cursorFunction = nullptr;
	keyFunction = nullptr;
	mouseFunction = nullptr;
	scrollFunction = nullptr;
	focusFunction = nullptr;
	resizeFunction = nullptr;
}

TInstance::~TInstance()
{
	if (glfwInitResult)
		glfwTerminate();
}

int TInstance::GetGlfwInitResult()
{
	return glfwInitResult;
}

UINT TInstance::GenerateWindow(TrecPointer<TWindow>& window, TrecPointer<TFileShell> uiInterface, const TString& name, t_window_type type)
{
	TString useName(name);
	if (!useName.GetSize())
		useName.Set(L"Anagame Window");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	GLFWwindow* glfwWindow = glfwCreateWindow(640, 480, useName.GetRegString().c_str(), nullptr, nullptr);
	if (!glfwWindow)
		return 1;

	TrecPointer<DrawingBoard> board = TrecPointerKey::GetNewSelfTrecPointerAlt<DrawingBoard, TWindow>(glfwWindow);

	// To-Do: Other Initialization




	// End To-Do: Assuming initialization worked

	window = TrecPointerKey::ConvertPointer<DrawingBoard, TWindow>(board);

	UINT Rust = 0;
	for (; Rust < windows.Size(); Rust++)
	{
		if (!windows[Rust].Get())
		{
			windows[Rust] = window;
			break;
		}
	}

	if (Rust == windows.Size())
		windows.push_back(window);

	if (charFunction)
		glfwSetCharCallback(glfwWindow, charFunction);
	if (cursorFunction)
		glfwSetCursorPosCallback(glfwWindow, cursorFunction);
	if (keyFunction)
		glfwSetKeyCallback(glfwWindow, keyFunction);
	if (mouseFunction)
		glfwSetMouseButtonCallback(glfwWindow, mouseFunction);
	if (scrollFunction)
		glfwSetScrollCallback(glfwWindow, scrollFunction);
	if (focusFunction)
		glfwSetWindowFocusCallback(glfwWindow, focusFunction);
	if (resizeFunction)
		glfwSetWindowSizeCallback(glfwWindow, resizeFunction);

	return Rust;
}

void TInstance::SetCallbacks(GLFWcharfun charFun, GLFWcursorposfun cursorFun,
	GLFWkeyfun keyFun, GLFWmousebuttonfun mouseFun,
	GLFWscrollfun scrollFun, GLFWwindowfocusfun focusFun,
	GLFWwindowsizefun resizeFun) {

	charFunction = charFun;
	cursorFunction = cursorFun;
	keyFunction = keyFun;
	mouseFunction = mouseFun;
	scrollFunction = scrollFun;
	focusFunction = focusFun;
	resizeFunction = resizeFun;
}

