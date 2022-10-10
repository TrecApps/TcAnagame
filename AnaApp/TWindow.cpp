#include "pch.h"
#include "TWindow.h"

TWindow::TWindow(GLFWwindow* window): DrawingBoard(window)
{
    needsRefresh = true;
    needsConstantRefresh = false;

    int w = 0; 
    int h = 0;
    area.top = h;
    area.left = w;
    glfwGetWindowSize(window, &w, &h);

    area.bottom = h;
    area.right = w;
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
}

void TWindow::SetMainPage(TrecPointer<TPage> mainPage)
{
    this->mainPage = mainPage;
}

void TWindow::OnMouseMove(double x, double y)
{
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
    message_output mOut = message_output::mo_negative;
    TDataArray<TPage::EventID_Cred> cred;

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
    message_output mOut = message_output::mo_negative;
    TDataArray<TPage::EventID_Cred> cred;

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

RECT_F TWindow::GetArea()
{
    return area;
}
