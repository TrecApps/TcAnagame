#include <GL/glew.h>
#include "TTextElement.h"
#include "TImageBrush.h"
#include "TColorBrush.h"

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

int powerOf2(int value)
{
	int ret = 1;
	while (ret < value) ret <<= 1;
	return ret;
}

void TTextElement::ClearHighlight()
{
	for (UINT Rust = 0; Rust < lines.Size(); Rust++)
	{
		auto& line = lines[Rust];
		for (UINT C = 0; C < line.characters.Size(); C++)
		{
			line.characters[C].isHighlighted = false;
		}
	}
}

void TTextElement::SetHighlight(UINT start, UINT end)
{
	ClearHighlight();
	for (UINT Rust = 0; Rust < lines.Size(); Rust++)
	{
		auto& line = lines[Rust];
		if (line.strIndex > end)
			return;
		
		for (UINT C = start - line.strIndex; C < line.characters.Size() && C < end - line.strIndex; C++)
			line.characters[C].isHighlighted = true;
	}
}

void TTextElement::AppendLine(BasicCharLine& curLine, float& y)
{
	curLine.totalWidth = 0.0f;
	for (UINT C = 0; C < curLine.characters.Size(); C++)
	{
		BasicCharacter tempChar(curLine.characters[C]);
#ifdef _WINDOWS
		curLine.height = max(curLine.height, tempChar.location.bottom - tempChar.location.top);
#elif defined(__linux__) || (defined (__APPLE__) && defined (__MACH__))
		curLine.height = std::max(curLine.height, tempChar.location.bottom - tempChar.location.top);
#endif
		curLine.totalWidth += (tempChar.location.right - tempChar.location.left);
	}
	curLine.top = y;
	lines.push_back(curLine);
	y += curLine.height + curLine.floorPadding;
	curLine = BasicCharLine();
	
}

void TTextElement::JustifyLine(BasicCharLine& line, float difference)
{
	UINT spaceCount = 0;
	for (UINT Rust = 0; Rust < line.characters.Size(); Rust++)
	{
		if (line.characters[Rust].character == L'\s')
			spaceCount++;
	}
	if (!spaceCount)
		return;
	difference /= spaceCount;

	float shift = 0.0f;

	for (UINT Rust = 0; Rust < line.characters.Size(); Rust++)
	{
		auto& character = line.characters[Rust];

		character.location.left += shift;
		character.location.right += shift;

		if (character.character == L'\s')
		{
			character.location.right += difference;
			shift += difference;
		}
	}
}

UCHAR* TTextElement::textInGlFormat(FT_Bitmap& bitmap, int& targetWidth, int targetHeight)
{
	targetWidth = powerOf2(bitmap.width);
	targetHeight = powerOf2(bitmap.rows);

	UCHAR* ret = new UCHAR[targetWidth * targetHeight * 4];

	UINT bitmapDataCount = 0;

	for (int j = 0; j < targetHeight; j++) {
		for (int i = 0; i < targetWidth; i++) {

			if (i < (bitmap.width * 4))
			{
				ret[j * i] = bitmap.buffer[bitmapDataCount++];
			}
			else
				ret[j * i] = 0;
		}
	}
	return ret;
}

bool TTextElement::HitTestPoint(const TPoint& point, BOOL& isTrailingHit, BOOL& isInside, UINT& position)
{
	if(!DrawingBoard::IsContained(point, this->bounds))
		return false;

	for (UINT Rust = 0; Rust < this->lines.Size(); Rust++)
	{
		auto& line = lines[Rust];
		if (point.y >= line.top && point.y <= (line.top + line.height))
		{
			if (!line.characters.Size())
			{
				isTrailingHit = true;
				isInside = false;
				position = line.strIndex;
				return true;
			}

			for (UINT C = 0; C < line.characters.Size(); C++)
			{
				auto& ch = line.characters[C];
				if (!DrawingBoard::IsContained(point, ch.location))
					continue;
				isInside = true;
				position = line.strIndex + C;
				isTrailingHit = (ch.location.right - point.x) < (point.x - ch.location.left);
				return true;
			}

			isInside = false;
			isTrailingHit = true;
			position = line.strIndex + line.characters.Size();
			return true;
		}
	}

	return false;
}

TTextElement::TTextElement(TrecPointer<DrawingBoard> board): drawingBoard(board)
{
	wrap = true;
	isClickDown = false;
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

RECT_F TTextElement::GetBounds()
{
	return bounds;
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
	UINT startIndex = 0;
	for (UINT Rust = 0; Rust < theText.GetSize() && y < bounds.bottom; Rust++)
	{
		BasicCharacter ch;
		ch.character = theText.GetAt(Rust);

		if (ch.character == L'\r')
		{
			curLine.strIndex = startIndex;
			startIndex = Rust;
			AppendLine(curLine, y);
		}
		if (ch.character == L'\n')
		{
			if (prevChar != L'\r')
			{
				curLine.strIndex = startIndex;
				startIndex = Rust;
				AppendLine(curLine, y);
			}
		}

		prevChar = ch.character;

		FT_UInt glyphIndex = FT_Get_Char_Index(curFace, ch.character);
		if(!glyphIndex)continue;

		if (FT_Load_Glyph(curFace, glyphIndex, FT_LOAD_COLOR))
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
				curLine.strIndex = startIndex;
				startIndex = Rust;
				AppendLine(curLine, y);
				curLine.isCarryOver = true;
			}
		}
	}

	if (!theText.EndsWith(L'\n'))
	{
		curLine.strIndex = startIndex;
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

void TTextElement::OnDraw(TrecPointer<TVariable> dataText)
{
	if (!drawingBoard.Get())
		return;
	float* verticies = TImageBrush::GeneratePictureVertices(this->bounds, this->bounds);
	if (!verticies)
		return;
	// First, Draw the Backgrounds or highlights
	for (UINT Rust = 0; Rust < lines.Size(); Rust++)
	{
		BasicCharLine& line = lines[Rust];
		for (UINT C = 0; C < line.characters.Size(); C++)
		{
			BasicCharacter& ch = line.characters[C];

			if (ch.isHighlighted)
			{
				auto brush = drawingBoard->GetHighlightBrush();
				auto solidBrush = TrecPointerKey::ConvertPointer<TBrush, TColorBrush>(brush);
				solidBrush->FillRectangle(ch.location);
			}
			else if (ch.backgroundColor.Get())
			{
				auto brush = drawingBoard->GetSolidColorBrush(*ch.backgroundColor.Get());
				auto solidBrush = TrecPointerKey::ConvertPointer<TBrush, TColorBrush>(brush);
				solidBrush->FillRectangle(ch.location);
			}

		}
	}



	drawingBoard->SetShader(TrecPointer<TShader>(), shader_type::shader_texture);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float[32]), verticies, GL_STATIC_DRAW);

	glBindVertexArray(VBO);

	for (UINT Rust = 0; Rust < lines.Size(); Rust++)
	{
		BasicCharLine& line = lines[Rust];
		for (UINT C = 0; C < line.characters.Size(); C++)
		{
			BasicCharacter& ch = line.characters[C];
			int width = 0, height = 0;
			UCHAR* charData = textInGlFormat(ch.bitmap, width, height);

			UINT textId = 0;
			glGenTextures(1, &textId);
			glBindTexture(GL_TEXTURE_2D, textId);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, charData);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			delete[] charData;
			charData = nullptr;

			glDeleteTextures(1, &textId);
		}
	}
}

bool TTextElement::GetMinHeight(float& height)
{
	float top = std::numeric_limits<float>::infinity();
	float bottom = 0.0f;

	for (UINT Rust = 0; Rust < lines.Size(); Rust++)
	{
		auto& line = lines[Rust].characters;
		for (UINT C = 0; C < line.Size(); C++)
		{
			auto& ch = line[C];
			if (top > ch.location.top)
				top = ch.location.top;

			if (bottom < ch.location.bottom)
				bottom = ch.location.top;
		}
	}

	if(!bottom || top == std::numeric_limits<double>::infinity() || top > bottom)
	return false;

	height = bottom - top;
	return true;
}

void updateWidth(float& base, float& compare)
{
	base += compare;
	compare = 0.0f;
}

int TTextElement::GetMinWidth(float& width, int doWrap)
{
	bool assumeWrap = false;

	if (!doWrap)
		assumeWrap = wrap;
	if (doWrap > 0)
		assumeWrap = true;

	width = 0.0f;

	for (UINT Rust = 0; Rust < lines.Size(); Rust++)
	{
		auto& line = lines[Rust].characters;
		float curWidth = 0;

		for (UINT C = 0; C < line.Size(); C++)
		{
			auto& ch = line[C];
			if (IsWhitespace(ch.character) && assumeWrap)
			{
				updateWidth(width, curWidth);
				continue;
			}
			curWidth += (ch.location.right - ch.location.left);
		}
		updateWidth(width, curWidth);
	}

	return wrap ? 1 : -1;
}

bool TTextElement::OnCLickDown(const TPoint& point)
{
	if (DrawingBoard::IsContained(point, bounds))
	{
		this->highlightRange.Reset();

		UINT mets = 0;
		BOOL trailing = FALSE, inside = FALSE;
		if (HitTestPoint(point, trailing, inside, mets))
		{
			UINT pos = trailing ? 1 : 0;
			pos += mets;

			this->highlightRange.SetBegin(pos);
			isClickDown = true;
		}


		return true;
	}
	return false;
}

bool TTextElement::OnCLickUp(const TPoint& point)
{
	isClickDown = false;
	if (DrawingBoard::IsContained(point, bounds))
	{
		UINT mets = 0;
		BOOL trailing = FALSE, inside = FALSE;
		if (HitTestPoint(point, trailing, inside, mets))
		{
			UINT pos = trailing ? 1 : 0;
			pos += mets;
			UINT start = 0, end = 0;
			ClearHighlight();

			this->highlightRange.SetEnd(pos);

			// Range Highlighter
			if (this->highlightRange.GetHighlightRange(start, end))
			{
				this->SetHighlight(start, end);
			}
		}

		return true;
	}
	return false;
}

bool TTextElement::OnMouseMove(const TPoint& point)
{
	if (DrawingBoard::IsContained(point, bounds))
	{
		UINT mets = 0;
		BOOL trailing = FALSE, inside = FALSE;
		if (isClickDown && HitTestPoint(point, trailing, inside, mets))
		{
			UINT pos = trailing ? 1 : 0;
			pos += mets;
			UINT start = 0, end = 0;
			//if (this->highlightRange.GetHighlightRange(start, end))
			//	this->mainLayout->SetDrawingEffect(nullptr, DWRITE_TEXT_RANGE{ start, end - start });

			this->highlightRange.SetEnd(pos);

			// Range Highlighter
			if (this->highlightRange.GetHighlightRange(start, end))
			{
				this->SetHighlight(start, end);
			}
		}
		return true;

	}
	return false;
}

void TTextElement::OnCutCopyPaste(control_text_mode mode)
{
	UINT begin = 0;
	UINT end = 0;
	if (highlightRange.GetHighlightRange(begin, end) && (mode == control_text_mode::ctm_cut || mode == control_text_mode::ctm_copy))
		glfwSetClipboardString(nullptr, text->GetString().SubString(begin, end).GetRegString().c_str());
}

bool TTextElement::OnInputChar(WCHAR ch, UINT count)
{
	return false;
}

bool TTextElement::SetText(const TString& text)
{
	this->text = TrecPointerKey::ConvertPointer<TVariable, TStringVariable>( 
		TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(text));
	ReCreateLayout();
	return true;
}

void TTextElement::GetText(TString& text)
{
	text.Empty();
	if (this->text.Get())
		text.Set(this->text->GetString());
}

bool TTextElement::LockText(bool doLock)
{
	return false;
}

bool TTextElement::TakesInput()
{
	return false;
}

bool TTextElement::CanTakeInput()
{
	return false;
}

void TTextElement::OnLoseFocus()
{
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
	top = 0;
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
#if _WINDOWS
	return max(0.0f, isFloor ? floorPadding : ceilingPadding);
#elif defined(__linux__) || (defined (__APPLE__) && defined (__MACH__))
	return std::max( 0.0f, isFloor ? floorPadding : ceilingPadding);
#endif
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
	isHighlighted = false;
	FT_Bitmap_Init(&bitmap);
}

BasicCharacter::~BasicCharacter()
{
	FT_Bitmap_Done(freeTypeLibrary, &bitmap);
}



HighlightRange::HighlightRange()
{
	Reset();
}

HighlightRange::HighlightRange(const HighlightRange& range)
{
	begin = range.begin;
	end = range.end;
	beginSet = range.beginSet;
	endSet = range.endSet;
}

void HighlightRange::Reset()
{
	this->begin = this->end = 0;
	this->beginSet = this->endSet = false;
}

bool HighlightRange::GetCarotLocation(UINT& loc)
{
	bool ret = begin == end && beginSet && endSet;
	if (ret)
		loc = begin;
	return ret;
}

bool HighlightRange::GetHighlightRange(UINT& begin, UINT& end)
{
	if (!beginSet || !endSet || (this->begin == this->end))
		return false;
	begin = this->begin;
	end = this->end;
	if (end < begin)
	{
		UINT temp = end;
		end = begin;
		begin = temp;
	}
	return true;
}

void HighlightRange::SetBegin(UINT begin)
{
	Reset();
	this->begin = begin;
	this->beginSet = true;
}

bool HighlightRange::SetEnd(UINT end)
{
	if (!beginSet)
		return false;
	this->end = end;
	this->endSet = true;
}
