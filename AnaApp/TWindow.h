#pragma once
#include <TString.h>
#include <DrawingBoard.h>
#include <TPoint.h>
#include "AnaApp.h"
#include <TPage.h>


class _ANA_APP TWindow :
    public DrawingBoard
{
    friend class TInstance;
    friend class TrecPointerKey;
    TPoint mousePoint;
protected:
    bool needsRefresh, needsConstantRefresh;

    TWindow(GLFWwindow* window);
    bool IsWindow(GLFWwindow* test);

    void OnDraw();

    TrecPointer<TPage> mainPage;
    TrecPointer<TPage::TScrollBar> currentScrollBar;

    RECT_F area;


    void HandleWindowEvents(TDataArray<TPage::EventID_Cred>& cred);

public:

    void SetMainPage(TrecPointer<TPage> mainPage);

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

    RECT_F GetArea();
};

