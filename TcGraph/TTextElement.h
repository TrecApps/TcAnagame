#pragma once
#include "DrawingBoard.h"
#include <TDataMap.h>
#include "GraphicsDef.h"

#include <ft2build.h>
#include FT_FREETYPE_H

class _TC_GRAPH TextFormattingDetails
{
public:
    TextFormattingDetails();
    TextFormattingDetails(const TextFormattingDetails& copy)=default;

    float fontSize;
    UCHAR formatTweaks;
};

class BasicCharacter
{
public:
    WCHAR character;
    RECT_F location;

    BasicCharacter() = default;
    BasicCharacter(const BasicCharacter& copy) = default;
};

class BasicCharLine
{
    friend class TTextElement;
    UINT height;
    UINT totalWidth;
    bool isCarryOver;
public:
    BasicCharLine() = default;
    BasicCharLine(const BasicCharLine& copy) = default;
    TDataArray<BasicCharacter> characters;
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
    TTextElement(TrecPointer<DrawingBoard> board);


    static bool InitializeText();
    bool RetrieveFont(const TString& name, FT_Face& face);
    static void ClearFonts();

    void SetBounds(RECT_F bounds);

    virtual void ReCreateLayout();
};

