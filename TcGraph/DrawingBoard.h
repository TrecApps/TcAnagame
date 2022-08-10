#pragma once

#include "TBrush.h"

class _TC_GRAPH DrawingBoard :
    public TVObject
{
    GLFWwindow* window;
    TrecPointerSoft<DrawingBoard> self;
    TColor defaultClearColor;
protected:
    
    explicit DrawingBoard(GLFWwindow* window);
    bool mode2d;
    
public:
    ~DrawingBoard() override;

    void BeginDraw() const;
    void ConfirmDraw();

    void set2D();

    TrecPointer<TBrush> GetSolidColorBrush(const TColor& color);
};

