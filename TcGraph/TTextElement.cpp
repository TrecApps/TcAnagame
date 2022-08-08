#include "TTextElement.h"

FT_Library  freeTypeLibrary;

#ifdef _WINDOWS
TString fontDirectory(L"C:\\Windows\\Fonts\\");
#elif defined(__linux__)
TString fontDirectory(L"/usr/share/fonts/truetype/");
#elif (defined (__APPLE__) && defined (__MACH__))
TString fontDirectory(L"/System/Library/Fonts/Supplemental/");
#endif // _WINDOWS

TDataMap<FT_Face> fontMap;

TTextElement::TTextElement(TrecPointer<DrawingBoard> board)
{
	wrap = true;
	bounds.bottom = bounds.left = bounds.right = bounds.top = 0.0f;
}

bool TTextElement::InitializeText()
{
	FT_Error error = FT_Init_FreeType(&freeTypeLibrary);


	return !error;
}

bool TTextElement::RetrieveFont(const TString& name, FT_Face& face)
{
	if (fontMap.retrieveEntry(name, face))
		return true;

	TString fileName(name + L".ttf");

#ifndef defined(__linux__)
	fileName.SetUpper(1);
#endif // !defined(__linux__)

	TString fontPath(fontDirectory + fileName);


	FT_Error error = FT_New_Face(freeTypeLibrary, fontPath.GetRegString().c_str(), 0, &face);

	if (error)
	{
		return false;
	}
	else
	{
		fontMap.addEntry(name, face);
	}

	return true;
}

void TTextElement::ClearFonts()
{
	TDataEntry<FT_Face> entry;
	UINT Rust = 0;
	while (fontMap.GetEntryAt(Rust++, entry))
	{
		FT_Done_Face(entry.object);
	}
	fontMap.clear();
}

void TTextElement::ReCreateLayout()
{
}

TextFormattingDetails::TextFormattingDetails()
{
}

TextFormattingDetails::TextFormattingDetails(const TextFormattingDetails& copy)
{
}
