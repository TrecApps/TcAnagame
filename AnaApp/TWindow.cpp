#include "pch.h"
#include "TWindow.h"

TWindow::TWindow(GLFWwindow* window) : DrawingBoard(window)
{
    isClicked = locked = false;
}

bool TWindow::IsWindow(GLFWwindow* test)
{
    return this->window == test;
}

void TWindow::OnDraw()
{
    if (needsRefresh || needsConstantRefresh)
    {
        this->BeginDraw();

        if (mainPage.Get())
            mainPage->Draw(TrecPointer<TVariable>());

        this->ConfirmDraw();
    }
    needsRefresh = false;
}

void TWindow::HandleWindowEvents(TDataArray<TPage::EventID_Cred>& cred)
{
    for (UINT Rust = 0; Rust < cred.Size(); Rust++)
    {
        if (cred[Rust].textIntercepter.Get())
            this->caret.intercepter = cred[Rust].textIntercepter;
        if (cred[Rust].eventType == R_Message_Type::On_Redraw)
            needsRefresh = true;
    }

}
//
//TrecPointer<TInstance> TWindow::GetInstance()
//{
//    return TrecPointerKey::TrecFromSoft<>(instance);
//}

TrecPointer<DrawingBoard> TWindow::GetDrawingBoard()
{
    return TrecPointerKey::TrecFromSoft<DrawingBoard>(this->self);
}

void TWindow::SetMainPage(TrecPointer<TPage> mainPage)
{
    this->mainPage = mainPage;
}

void TWindow::OnChar(UINT ch)
{
    if (locked) return;
    if (caret.intercepter.Get())
        caret.intercepter->OnChar(ch, 0, 0);
}

void TWindow::OnMouseMove(double x, double y)
{
    if (locked) return;
    if (!isClicked)
    {
        this->SetCursor(ag_mouse_pointer::standard);
    }
    mousePoint.x = x;
    mousePoint.y = y;

    message_output mOut = message_output::mo_negative;
    TDataArray<TPage::EventID_Cred> cred;
    
    if (currentScrollBar.Get()) {
        currentScrollBar->OnMouseMove(0, mousePoint, mOut);
        this->needsRefresh = true;
        return;
    }

    if (mainPage.Get())
    {

        mainPage->OnMouseMove(0, mousePoint, mOut, cred);
    }

    HandleWindowEvents(cred);
}

void TWindow::OnLButtonDown(int mods)
{
    if (locked) return;
    message_output mOut = message_output::mo_negative;
    TDataArray<TPage::EventID_Cred> cred;

    isClicked = true;

    if (currentScrollBar.Get()) {
        currentScrollBar->OnLButtonDown(0, mousePoint, mOut);
        this->needsRefresh = true;
        return;
    }

    if (mainPage.Get())
    {

        mainPage->OnLButtonDown(0, mousePoint, mOut, cred);
    }

    HandleWindowEvents(cred);
}

void TWindow::OnLButtonUp(int mods)
{
    if (locked) return;
    message_output mOut = message_output::mo_negative;
    TDataArray<TPage::EventID_Cred> cred;
    isClicked = false;

    if (currentScrollBar.Get()) {
        currentScrollBar->OnLButtonUp(0, mousePoint, mOut);
        this->needsRefresh = true;
        return;
    }

    if (mainPage.Get())
    {

        mainPage->OnLButtonUp(0, mousePoint, mOut, cred);
    }

    HandleWindowEvents(cred);
}

void TWindow::OnRButtonDown(int mods)
{
    if (locked) return;
    message_output mOut = message_output::mo_negative;
    TDataArray<TPage::EventID_Cred> cred;
    if (mainPage.Get())
    {

        mainPage->OnRButtonDown(0, mousePoint, mOut, cred);
    }

    HandleWindowEvents(cred);
}

void TWindow::OnRButtonUp(int mods)
{
    if (locked) return;
    message_output mOut = message_output::mo_negative;
    TDataArray<TPage::EventID_Cred> cred;

    if (mainPage.Get())
    {

        mainPage->OnRButtonUp(0, mousePoint, mOut, cred);
    }

    HandleWindowEvents(cred);
}

void TWindow::OnScroll(double x, double y)
{
    if (locked) return;
    if (mainPage.Get())
    {
        TDataArray<TPage::EventID_Cred> cred;
        
        mainPage->OnScroll(false, mousePoint, TPoint(x, y), cred);
        HandleWindowEvents(cred);
    }
    
}

void TWindow::OnFocus()
{
}

void TWindow::OnLoseFocus()
{
}

void TWindow::OnResize(int w, int h)
{
    area.right = w;
    area.bottom = h;

    if (mainPage.Get())
    {
        TDataArray<TPage::EventID_Cred> cred;
        mainPage->OnResize(area, 0, cred);
        HandleWindowEvents(cred);
    }
}

bool TWindow::Close()
{
    return true;
}

void TWindow::LockWindow()
{
    TObjectLocker lock(&thread);
    locked = true;
}

void TWindow::UnlockWindow()
{
    TObjectLocker lock(&thread);
    locked = false;
}

