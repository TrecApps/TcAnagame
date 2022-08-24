#include "TTextElement.h"

FT_Library  freeTypeLibrary;

#ifdef _WINDOWS
const TString fontDirectory(L"C:\\Windows\\Fonts\\");
#elif defined(__linux__)
TString fontDirectory(L"/usr/share/fonts/truetype/");
#elif (defined (__APPLE__) && defined (__MACH__))
TString fontDirectory(L"/System/Library/Fonts/Supplemental/");
#endif // _WINDOWS

#define FONT_WEIGHT_ARRAY_SIZE 9

const int hundredMinWeight[FONT_WEIGHT_ARRAY_SIZE] = {
	850,
	750,
	650,
	550,
	450,
	350,
	250,
	150,
	50
};

const int resultWeight[FONT_WEIGHT_ARRAY_SIZE] = {
	4,
	3,
	2,
	1,
	0,
	-1,
	-2,
	-3,
	-4
};

static TDataMap<FT_Face> fontMap;

TTextElement::TTextElement(TrecPointer<DrawingBoard> board): drawingBoard(board)
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
		int width = 300;
		int height = 300;
		drawingBoard->GetDisplayResolution(width, height);
		FT_Set_Char_Size(face, 0, 16*64, width, height);
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

void TTextElement::SetBounds(RECT_F bounds)
{
	this->bounds = bounds;
	ReCreateLayout();
}

void TTextElement::ReCreateLayout()
{
}

TextFormattingDetails::TextFormattingDetails():
	fontSize(16.0f),
	formatTweaks(0)
{
}

BasicCharLine::BasicCharLine()
{
	height = totalWidth = 0;
	isCarryOver = false;
}

void BasicCharLine::SetLineSpacing(tc_line_spacing spacing)
{
	attributes = (attributes & 0b11111100) + static_cast<UCHAR>(spacing);
}

tc_line_spacing BasicCharLine::GetLineSpacing() const
{
	return static_cast<tc_line_spacing>(attributes & 0x3);
}

void BasicCharLine::SetVerticalPadding(float value, bool isFloor)
{
	if (isFloor)
		this->floorPadding = value;
	else this->ceilingPadding = value;
}

float BasicCharLine::GetVerticalPadding(bool isFloor) const
{

	return max( 0.0f, isFloor ? floorPadding : ceilingPadding);
}

int BasicCharacter::GetWeightStrength() const 
{
	bool negative = format & 0x8;
	int ret = format & 0x7;
	if (ret > 4)
		ret = 4; // Max value is 4
	if (negative) ret = (-ret);
	return ret;
}

void BasicCharacter::SetWeightStrength(int weight)
{
	for (UINT Rust = 0; Rust < FONT_WEIGHT_ARRAY_SIZE; Rust++)
	{
		if (weight > hundredMinWeight[Rust])
		{
			weight = resultWeight[Rust];
			break;
		}
	}

	bool negative = weight & 0x8;
	format = (format & 0xf0) + (weight & 0x7);
	if (negative)
		format |= 0x8;
}

BasicCharacter::BasicCharacter()
{
	character = 0;
	location = { 0,0,0,0 };
	format = 0;
}
