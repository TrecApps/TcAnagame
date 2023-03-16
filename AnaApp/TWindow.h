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

    TWindow(GLFWwindow* window);
    bool IsWindow(GLFWwindow* test);

    void OnDraw();

    TrecPointer<TPage> mainPage;
    TrecPointer<TPage::TScrollBar> currentScrollBar;

    void HandleWindowEvents(TDataArray<TPage::EventID_Cred>& cred);

    bool isClicked;

    bool locked;

    //TrecPointerSoft<TInstance> instance;

public:

    //TrecPointer<TInstance> GetInstance();

    TrecPointer<DrawingBoard> GetDrawingBoard();

    virtual void SetMainPage(TrecPointer<TPage> mainPage);

    void OnChar(UINT ch, bool fromChar = true);
    void OnMouseMove(double x, double y);
    void OnLButtonDown(int mods);
    void OnLButtonUp(int mods);
    void OnRButtonDown(int mods);
    void OnRButtonUp(int mods);
    void OnScroll(double x, double y);
    void OnFocus();
    void OnLoseFocus();
    void OnBufferResize(int w, int h);
    virtual void OnResize(int w, int h);
    virtual bool Close();

    void LockWindow();
    void UnlockWindow();

};

