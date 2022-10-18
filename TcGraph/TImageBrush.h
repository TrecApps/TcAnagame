#pragma once
#include "TBrush.h"
#include "DrawingBoard.h"
#include "GraphicsDef.h"



class _TC_GRAPH TImageBrush :
    public TBrush
{
    friend class DrawingBoard;
    TrecPointerSoft<DrawingBoard> window;

    GLuint textureId;
    int width, height, channelCount;

    UCHAR* data;

    void GenerateImageData(TrecPointer<TFileShell> file);

public:
    ~TImageBrush();

    static float* GeneratePictureVertices(const RECT_F& picLocation, const RECT_F& clipLocation);
    static bool NormalizeRect(RECT_F& output, const RECT_F& input, TrecPointer<DrawingBoard> board);

    brush_type GetBrushType() override;

    void FillRectangle(const RECT_F& location);
    void FillRectangle(const RECT_F& picLocation, const RECT_F& clipLocation);
};

