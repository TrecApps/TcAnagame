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

void TTextElement::AppendLine(BasicCharLine& curLine, float& y)
{
	curLine.totalWidth = 0.0f;
	for (UINT C = 0; C < curLine.characters.Size(); C++)
	{
		BasicCharacter tempChar(curLine.characters[C]);
		curLine.height = max(curLine.height, tempChar.location.bottom - tempChar.location.top);
		curLine.totalWidth += (tempChar.location.right - tempChar.location.left);
	}
	curLine.top = y;
	lines.push_back(curLine);
	y += curLine.height + curLine.floorPadding;
	curLine = BasicCharLine();
	
}

void TTextElement::JustifyLine(BasicCharLine& line, float difference)
{
}

TTextElement::TTextElement(TrecPointer<DrawingBoard> board): drawingBoard(board)
{
	wrap = true;
	bounds.bottom = bounds.left = bounds.right = bounds.top = 0.0f;

	text = TrecPointerKey::ConvertPointer<TVariable, TStringVariable>(
		TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(L""));
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

void TTextElement::SetBounds(RECT_F bounds1)
{
	this->bounds = bounds1;
	ReCreateLayout();
}

void TTextElement::ReCreateLayout()
{
	lines.RemoveAll();

	BasicCharLine curLine;

	const TString& theText = text->GetString();

	FT_Face curFace = nullptr;
	if (!RetrieveFont(formattingDetails.font, curFace))
		return;

	int width = 300;
	int height = 300;
	drawingBoard->GetDisplayResolution(width, height);
	FT_Set_Char_Size(curFace, 0, 16 * 64, width, height);

	float x = bounds.left;
	float y = bounds.top;
	WCHAR prevChar = L'\0';

	for (UINT Rust = 0; Rust < theText.GetSize() && y < bounds.bottom; Rust++)
	{
		BasicCharacter ch;
		ch.character = theText.GetAt(Rust);

		if (ch.character == L'\r')
		{
			AppendLine(curLine, y);
		}
		if (ch.character == L'\n')
		{
			if (prevChar != L'\r')
				AppendLine(curLine, y);
		}

		prevChar = ch.character;

		FT_UInt glyphIndex = FT_Get_Char_Index(curFace, ch.character);
		if(!glyphIndex)continue;

		if (FT_Load_Glyph(curFace, glyphIndex, FT_LOAD_DEFAULT))
			continue;

		if(FT_Render_Glyph(curFace->glyph, FT_RENDER_MODE_NORMAL))continue;

		auto weight = ch.GetWeightStrength();
		FT_Bitmap_Embolden(freeTypeLibrary, &(curFace->glyph->bitmap), weight, weight);
		FT_Vector offset{0,0};
		FT_Color color{
			static_cast<UCHAR>(formattingDetails.defaultTextColor.GetBlue() * 255), // In FT_Color, blue is first
			static_cast<UCHAR>(formattingDetails.defaultTextColor.GetGreen() * 255), // In FT_Color, green is second
			static_cast<UCHAR>(formattingDetails.defaultTextColor.GetRed() * 255), // In FT_Color, red is third
			static_cast<UCHAR>(formattingDetails.defaultTextColor.GetOpacity() * 255), // In FT_Color, alpha is last
		};
		
		FT_Bitmap_Blend(freeTypeLibrary, &(curFace->glyph->bitmap), { 0,0 }, &ch.bitmap, &offset, color);

		ch.location.top = y;
		ch.location.bottom = y + curFace->glyph->advance.y;

		ch.location.left = x;
		x += curFace->glyph->advance.x;
		ch.location.right = x;

		ch.backgroundColor = formattingDetails.defaultBackgroundColor;
		
		curLine.characters.push_back(ch);

		if (x > bounds.right)
		{
			if (wrap)
			{
				AppendLine(curLine, y);
				curLine.isCarryOver = true;
			}
		}
	}

	if (!theText.EndsWith(L'\n'))
	{
		AppendLine(curLine, y);
	}

	if (!lines.Size())
		return;

	// Address Paragraph (vertical) spacing

	float bottomDifference = bounds.bottom - y;

	bool updateSpacing = false;

	if (bottomDifference > 0)
	{
		switch (formattingDetails.textSpacing)
		{
		case tc_text_spacing::center:
			bottomDifference /= 2;
		case tc_text_spacing::bottom:
			updateSpacing = true;
		}
	}

	if (updateSpacing)
	{
		for (UINT Rust = 0; Rust < lines.Size(); Rust++)
		{
			lines[Rust].top += bottomDifference;
			for (UINT C = 0; C < lines[Rust].characters.Size(); C++)
			{
				BasicCharacter& ch = lines[Rust].characters[C];
				ch.location.bottom += bottomDifference;
				ch.location.top += bottomDifference;
			}
		}
	}

	// Address Line (horizontal) Spacing

	if (formattingDetails.defaultLineSpacing == tc_line_spacing::left)
		return; // Nothing to Do

	float boundsWidth = bounds.right - bounds.left;

	for (UINT Rust = 0; Rust < lines.Size(); Rust++)
	{
		bottomDifference = boundsWidth - lines[Rust].totalWidth;
		if (!(bottomDifference > 0.0f))
			continue;

		switch (formattingDetails.defaultLineSpacing)
		{
		case tc_line_spacing::center:
			bottomDifference /= 2;
			break;
		case tc_line_spacing::justified:
			JustifyLine(lines[Rust], bottomDifference);
			continue;
		}

		for (UINT C = 0; C < lines[Rust].characters.Size(); Rust++)
		{
			BasicCharacter& ch = lines[Rust].characters[C];
			ch.location.left += bottomDifference;
			ch.location.right += bottomDifference;
		}
	}
}

TextFormattingDetails::TextFormattingDetails():
	fontSize(16.0f),
	formatTweaks(0),
	defaultLineSpacing(tc_line_spacing::left),
	textSpacing(tc_text_spacing::center)
{
	font.Set(L"Arial");
}

BasicCharLine::BasicCharLine()
{
	height = totalWidth = 0;
	isCarryOver = false;

	attributes = 0;
	ceilingPadding = floorPadding = 0.0f;
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
	FT_Bitmap_Init(&bitmap);
}

BasicCharacter::~BasicCharacter()
{
	FT_Bitmap_Done(freeTypeLibrary, &bitmap);
}
