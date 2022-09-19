#include <GLFW/glfw3.h>
#include "TInstance.h"

TrecPointer<TWindow> TInstance::GetWindow(GLFWwindow* win)
{
	for (UINT Rust = 0; Rust < windows.Size(); Rust++)
	{
		if (windows[Rust].Get() && windows[Rust]->IsWindow(win))
			return windows[Rust];
	}
	return TrecPointer<TWindow>();
}

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

	windowCount = 0;
}

TInstance::~TInstance()
{
	if (glfwInitResult)
		glfwTerminate();
}

void TInstance::DoDraw()
{
	for (UINT Rust = 0; Rust < windows.Size(); Rust++)
	{
		if (windows[Rust].Get())
			windows[Rust]->OnDraw();
	}
}

int TInstance::GetGlfwInitResult()
{
	return glfwInitResult;
}

UINT TInstance::HasWindows()
{
	return windowCount;
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

	TrecPointer<DrawingBoard> board = TrecPointerKey::GetNewTrecPointerAlt<DrawingBoard, TWindow>(glfwWindow);

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

	windowCount++;

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
	GLFWwindowsizefun resizeFun, GLFWwindowclosefun closeFun) {

	charFunction = charFun;
	cursorFunction = cursorFun;
	keyFunction = keyFun;
	mouseFunction = mouseFun;
	scrollFunction = scrollFun;
	focusFunction = focusFun;
	resizeFunction = resizeFun;
	closeFunction = closeFun;
}

void TInstance::OnChar(GLFWwindow* win, UINT ch)
{
}

void TInstance::OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{
	auto win = GetWindow(window);
	if (win.Get())
		win->OnMouseMove(xpos, ypos);
}

void TInstance::OnLButtonDown(GLFWwindow* window, int mods)
{
	auto win = GetWindow(window);
	if (win.Get())
		win->OnLButtonDown(mods);
}

void TInstance::OnLButtonUp(GLFWwindow* window, int mods)
{
	auto win = GetWindow(window);
	if (win.Get())
		win->OnLButtonUp(mods);
}

void TInstance::OnRButtonDown(GLFWwindow* window, int mods)
{
	auto win = GetWindow(window);
	if (win.Get())
		win->OnRButtonDown(mods);
}

void TInstance::OnRButtonUp(GLFWwindow* window, int mods)
{
	auto win = GetWindow(window);
	if (win.Get())
		win->OnLButtonUp(mods);
}

void TInstance::OnScroll(GLFWwindow* window, double x, double y)
{
	auto win = GetWindow(window);
	if (win.Get())
		win->OnScroll(x, y);
}

void TInstance::OnFocus(GLFWwindow* window)
{
	auto win = GetWindow(window);
	if (win.Get())
		win->OnFocus();
}

void TInstance::OnLoseFocus(GLFWwindow* window)
{
	auto win = GetWindow(window);
	if (win.Get())
		win->OnLoseFocus();
}

void TInstance::OnWindowResize(GLFWwindow* window, int w, int h)
{
	auto win = GetWindow(window);
	if (win.Get())
		win->OnResize(w, h);
}

void TInstance::OnWindowClose(GLFWwindow* window)
{
	if (!window)
		return;
	for (UINT Rust = 0; Rust < windows.Size(); Rust++)
	{
		if (!windows[Rust].Get()) continue;
		if (windows[Rust]->IsWindow(window) && windows[Rust]->Close())
		{
			windowCount--;
			windows[Rust].Nullify();
			return;
		}
	}
}

