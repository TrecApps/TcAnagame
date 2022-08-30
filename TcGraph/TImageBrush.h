#pragma once
#include "TBrush.h"
#include "DrawingBoard.h"



class TImageBrush :
    public TBrush
{
    friend class DrawingBoard;
    TrecPointerSoft<DrawingBoard> window;

    GLuint textureId;
    int width, height, channelCount;

    UCHAR* data;

public:
    ~TImageBrush();
};

