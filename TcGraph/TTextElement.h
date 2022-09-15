#pragma once
#include "DrawingBoard.h"
#include <TDataMap.h>
#include "GraphicsDef.h"

#include <ft2build.h>
#include <TPoint.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H
#include FT_BITMAP_H
#include FT_STROKER_H

using tc_line_spacing = enum class tc_line_spacing
{
    left = 0,
    right = 1,
    center = 2,
    justified = 3
};

using tc_text_spacing = enum class tc_text_spacing
{
    top = 0,
    center = 1,
    bottom = 2
};

class _TC_GRAPH TextFormattingDetails
{
public:
    TextFormattingDetails();
    TextFormattingDetails(const TextFormattingDetails& copy)=default;

    float fontSize;
    UCHAR formatTweaks;
    TString font;
    tc_line_spacing defaultLineSpacing;
    tc_text_spacing textSpacing;
    TColor defaultTextColor;
    TrecPointer<TColor> defaultBackgroundColor;
};

class BasicCharacter
{
public:
    WCHAR character;
    RECT_F location;
    TrecPointer<TColor> backgroundColor;
    FT_Bitmap bitmap;
    UCHAR format;

    int GetWeightStrength()const;
    void SetWeightStrength(int weight);

    BasicCharacter();
    BasicCharacter(const BasicCharacter& copy) = default;
    ~BasicCharacter();
};

class BasicCharLine
{
    friend class TTextElement;
    float top;
    float height;
    float totalWidth;
    bool isCarryOver;
    float ceilingPadding;
    float floorPadding;

    UCHAR attributes;

    UINT strIndex;

public:
    BasicCharLine();
    BasicCharLine(const BasicCharLine& copy) = default;
    TDataArray<BasicCharacter> characters;

    void SetLineSpacing(tc_line_spacing spacing);
    tc_line_spacing GetLineSpacing() const;

    void SetVerticalPadding(float value, bool isFloor = true);
    float GetVerticalPadding(bool isFloor = true) const;
};

class _TC_GRAPH HighlightRange
{
public:
    HighlightRange();
    HighlightRange(const HighlightRange& range);

    bool beginSet, endSet;
    UINT begin, end;

    void Reset();
    bool GetCarotLocation(UINT& loc);
    bool GetHighlightRange(UINT& begin, UINT& end);

    void SetBegin(UINT begin);
    bool SetEnd(UINT end);
};

class _TC_GRAPH TTextElement :
    public TVObject
{
    friend class TrecPointerKey;

protected:
    TrecPointer<DrawingBoard> drawingBoard;
    TrecPointer<TStringVariable> text;

    RECT_F bounds;
    bool wrap;

    TDataArray<BasicCharLine> lines;

    TextFormattingDetails formattingDetails;

    HighlightRange highlightRange;
    bool isClickDown;

    void AppendLine(BasicCharLine& line, float& y);

    void JustifyLine(BasicCharLine& line, float difference);

    UCHAR* textInGlFormat(FT_Bitmap& bitmap, int& targetWidth, int targetHeight);

    bool HitTestPoint(
       const TPoint& point,
        _Out_ BOOL& isTrailingHit,
        _Out_ BOOL& isInside,
        _Out_ UINT& position
        );

public:
    explicit TTextElement(TrecPointer<DrawingBoard> board);


    static bool InitializeText();
    bool RetrieveFont(const TString& name, FT_Face& face);
    static void ClearFonts();

    void SetBounds(RECT_F bounds);

    virtual void ReCreateLayout();
    virtual void OnDraw(TrecPointer<TVariable> dataText);

    bool GetMinHeight(float& height);
    int GetMinWidth(float& width, int doWrap);

    virtual bool OnCLickDown(const TPoint& point);
    virtual bool OnCLickUp(const TPoint& point);
    virtual bool OnMouseMove(const TPoint& point);
};

