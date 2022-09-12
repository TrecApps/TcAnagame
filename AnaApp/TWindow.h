#pragma once
#include <TString.h>
#include <DrawingBoard.h>
class TWindow :
    public DrawingBoard
{
    friend class TInstance;
protected:
    TWindow(GLFWwindow* window);

};

