#pragma once

#include "TcGraph.h"
#include <TString.h>

#include <GLFW/glfw3.h>

class _TC_GRAPH DrawingBoard :
    public TVObject
{
protected:
    GLFWwindow* window;
    TrecPointerSoft<DrawingBoard> self;
    DrawingBoard(GLFWwindow* window);
public:

    virtual ~DrawingBoard();


    void ConfirmDraw();


};

