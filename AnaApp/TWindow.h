#pragma once
#include <TString.h>
#include <DrawingBoard.h>
#include <TPoint.h>
class TWindow :
    public DrawingBoard
{
    friend class TInstance;
    TPoint mousePoint;
protected:
    bool needsRefresh, needsConstantRefresh;

    TWindow(GLFWwindow* window);
    bool IsWindow(GLFWwindow* test);

    void OnDraw();

public:

    void OnMouseMove(double x, double y);
    void OnLButtonDown(int mods);
    void OnLButtonUp(int mods);
    void OnRButtonDown(int mods);
    void OnRButtonUp(int mods);
    void OnScroll(double x, double y);
    void OnFocus();
    void OnLoseFocus();
    void OnResize(int w, int h);
    bool Close();
};

