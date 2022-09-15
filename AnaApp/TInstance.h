#pragma once
#include <TString.h>
#include "TWindow.h"
#include <GLFW/glfw3.h>

using t_window_type = enum class t_window_type
{
    t_window_type_plain,  // Gets a Plain Window
    t_window_type_ide,    // Gets a Basic IDE style Window
    t_window_type_web     // Window Optimized for Web-Browsing
};

class TInstance :
    public TVObject
{
    int glfwInitResult;
    TDataArray<TrecPointer<TWindow>> windows;

    GLFWcharfun charFunction;
    GLFWcursorposfun cursorFunction;
    GLFWkeyfun keyFunction;
    GLFWmousebuttonfun mouseFunction;
    GLFWscrollfun scrollFunction;
    GLFWwindowfocusfun focusFunction;
    GLFWwindowsizefun resizeFunction;

    TrecPointer<TWindow> GetWindow(GLFWwindow* win);

public:
    TInstance();
    ~TInstance();

    int GetGlfwInitResult();

    UINT GenerateWindow(TrecPointer<TWindow>& window, TrecPointer<TFileShell> uiInterface, const TString& name, t_window_type type = t_window_type::t_window_type_plain);

    void SetCallbacks(GLFWcharfun, GLFWcursorposfun, GLFWkeyfun, GLFWmousebuttonfun, GLFWscrollfun, GLFWwindowfocusfun, GLFWwindowsizefun);

    void OnChar(GLFWwindow* win, UINT ch);
    void OnMouseMove(GLFWwindow* window, double xpos, double ypos);
    void OnLButtonDown(GLFWwindow* window, int mods);
    void OnLButtonUp(GLFWwindow* window, int mods);
    void OnRButtonDown(GLFWwindow* window, int mods);
    void OnRButtonUp(GLFWwindow* window, int mods);
    void OnScroll(GLFWwindow* window, double x, double y);
    void OnFocus(GLFWwindow* window);
    void OnLoseFocus(GLFWwindow* window);
    void OnWindowResize(GLFWwindow* window, int w, int h);
};
