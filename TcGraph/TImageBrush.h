#pragma once
#include "TBrush.h"
#include "DrawingBoard.h"
#include "GraphicsDef.h"



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

    void FillRectangle(const RECT_F& location);
    void FillRectangle(const RECT_F& picLocation, const RECT_F& clipLocation);
};

