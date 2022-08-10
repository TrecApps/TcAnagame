#pragma once

#include "TcGraph.h"
#include <TString.h>

#include <GLFW/glfw3.h>

#include "TColor.h"

class _TC_GRAPH DrawingBoard :
    public TVObject
{
    GLFWwindow* window;
    TrecPointerSoft<DrawingBoard> self;
    TColor defaultClearColor;
protected:
    
    explicit DrawingBoard(GLFWwindow* window);

    
public:

    ~DrawingBoard() override;

    void BeginDraw() const;
    void ConfirmDraw();


};

