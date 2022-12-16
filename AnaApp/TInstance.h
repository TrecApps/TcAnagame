#pragma once
#include <TString.h>
#include "TWindow.h"
#include <GLFW/glfw3.h>
#include "AnaApp.h"

using t_window_type = enum class t_window_type
{
    t_window_type_plain,  // Gets a Plain Window
    t_window_type_ide,    // Gets a Basic IDE style Window
    t_window_type_web     // Window Optimized for Web-Browsing
};

using t_dialog_modal_mode = enum class t_dialog_modal_mode {

    // If you intend to get a result while using 'no_modal' or 'soft_modal' you should set a callback runner on the procedure to handle it
    no_modal,   // Parent Window continues to operate while Child Window is open
    soft_modal, // Parent window can still move around and be resized, but is otherwise disabled while Child Window is open
    hard_modal  // Parent Window is utterly disabled while the child window is open
};

class _ANA_APP TInstance :
    public TVObject
{
    friend class TrecPointerKey;

    int glfwInitResult;
    TDataArray<TrecPointer<TWindow>> windows;

    GLFWcharfun charFunction;
    GLFWcursorposfun cursorFunction;
    GLFWkeyfun keyFunction;
    GLFWmousebuttonfun mouseFunction;
    GLFWscrollfun scrollFunction;
    GLFWwindowfocusfun focusFunction;
    GLFWwindowsizefun resizeFunction;

    GLFWwindowclosefun closeFunction;

    TrecPointer<TWindow> GetWindow(GLFWwindow* win);

    UINT windowCount;

    TrecPointerSoft<TInstance> self;
    TInstance();
public:
    static TrecPointer<TInstance> GetInstance();
    ~TInstance();

    void SetSelf(TrecPointer<TInstance> newSelf);

    void DoDraw();

    int GetGlfwInitResult();

    UINT HasWindows();

    UINT GenerateWindow(TrecPointer<TWindow>& window, TrecPointer<TFileShell> uiInterface, const TString& name, t_window_type type = t_window_type::t_window_type_plain);
    UINT GenerateDialog(TrecPointer<TWindow>& window, TrecPointer<TWindow> parent,
        const TString& name, TrecPointer<TPageBuilder> page, 
        TrecPointer<TFileShell> uiInterface, const TString& details,
        t_dialog_modal_mode modalMode);

    void SetCallbacks(GLFWcharfun, GLFWcursorposfun, GLFWkeyfun, GLFWmousebuttonfun, GLFWscrollfun, GLFWwindowfocusfun, GLFWwindowsizefun, GLFWwindowclosefun);

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
    void OnWindowClose(GLFWwindow* window);
};

