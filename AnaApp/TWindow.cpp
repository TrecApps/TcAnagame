#include "pch.h"
#include "TWindow.h"

TWindow::TWindow(GLFWwindow* window): DrawingBoard(window)
{
}

bool TWindow::IsWindow(GLFWwindow* test)
{
    return this->window == test;
}

void TWindow::OnMouseMove(double x, double y)
{
    mousePoint.x = x;
    mousePoint.y = y;

}

void TWindow::OnLButtonDown(int mods)
{
}

void TWindow::OnLButtonUp(int mods)
{
}

void TWindow::OnRButtonDown(int mods)
{
}

void TWindow::OnRButtonUp(int mods)
{
}

void TWindow::OnScroll(double x, double y)
{
}

void TWindow::OnFocus()
{
}

void TWindow::OnLoseFocus()
{
}

void TWindow::OnResize(int w, int h)
{
}
