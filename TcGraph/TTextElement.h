#pragma once
#include "DrawingBoard.h"
#include <TDataMap.h>
#include "GraphicsDef.h"

#include <ft2build.h>
#include FT_FREETYPE_H

using tc_line_spacing = enum class tc_line_spacing
{
    left = 0,
    right = 1,
    center = 2,
    justified = 3
};

class _TC_GRAPH TextFormattingDetails
{
public:
    TextFormattingDetails();
    TextFormattingDetails(const TextFormattingDetails& copy)=default;

    float fontSize;
    UCHAR formatTweaks;
    tc_line_spacing defaultLineSPacing;
};

class BasicCharacter
{
public:
    WCHAR character;
    RECT_F location;
    TColor textColor;
    TrecPointer<TColor> backgroundColor;
    UCHAR format;

    int GetWeightStrength()const;
    void SetWeightStrength(int weight);

    BasicCharacter();
    BasicCharacter(const BasicCharacter& copy) = default;
};

class BasicCharLine
{
    friend class TTextElement;
    UINT height;
    UINT totalWidth;
    bool isCarryOver;
    float ceilingPadding;
    float floorPadding;
    UCHAR attributes;
public:
    BasicCharLine();
    BasicCharLine(const BasicCharLine& copy) = default;
    TDataArray<BasicCharacter> characters;

    void SetLineSpacing(tc_line_spacing spacing);
    tc_line_spacing GetLineSpacing() const;

    void SetVerticalPadding(float value, bool isFloor = true);
    float GetVerticalPadding(bool isFloor = true) const;
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

public:
    explicit TTextElement(TrecPointer<DrawingBoard> board);


    static bool InitializeText();
    bool RetrieveFont(const TString& name, FT_Face& face);
    static void ClearFonts();

    void SetBounds(RECT_F bounds);

    virtual void ReCreateLayout();
};

