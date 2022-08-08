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
    TextFormattingDetails(const TextFormattingDetails& copy);

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

public:
    TTextElement(TrecPointer<DrawingBoard> board);


    static bool InitializeText();
    static bool RetrieveFont(const TString& name, FT_Face& face);
    static void ClearFonts();

    virtual void ReCreateLayout();
};

