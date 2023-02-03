#include <GL/glew.h>
#include <GL/glew.h>
#include "TTextElement.h"
#include "TImageBrush.h"
#include "TColorBrush.h"
#include <cassert>
#include <TcRunner.h>
#include <atltrace.h>

FT_Library  freeTypeLibrary;





void SetFontCharacter(const TString& font, WCHAR ch, int weight, GLuint id, TrecPointer<DrawingBoard> board)
{
	std::map<CharWithSize, GLuint> map;
	board->RetrieveFontEntry(font, map);

	map.insert_or_assign(CharWithSize(ch, weight), id);
	board->SetFontEntry(font, map);
}

bool GetFontCharacter(const TString& font, WCHAR ch, int weight, GLuint& id, TrecPointer<DrawingBoard> board)
{
	std::map<CharWithSize, GLuint> map;
	if(!board->RetrieveFontEntry(font, map)) return false;

	auto it = map.find(CharWithSize(ch,weight));

	if (it == map.end())
		return false;
	id = it->second;
	return true;
}

//void EmptyMap() {
//	charMap.clear();
//}

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

/**
 * Class: TTextElementInterceter
 * Purpose: Intercepts text from handlers to the TTextElements
 */
class _TC_GRAPH TTextElementIntercepter : public TTextIntercepter
{
public:
	/**
	 * Method: TTextIntercepter::OnChar
	 * Purpose: Takes a character and feeds it to its target
	 * Parameters: WCHAR ch - the character to report
	 *          UINT count number of instances of that character to feed
	 *          UINT flags - flags (usually 0)
	 * Returns: void
	 *
	 * Attributes: abstract
	 */
	virtual void OnChar(UINT ch, UINT count, UINT flags)
	{
		element->OnInputChar(ch, count);
	}


	/**
	 * Method: TTextIntercepter::OnLoseFocus
	 * Purpose: Alerts the target that it will no longer be intercepting characters
	 * Parameters: void
	 * Returns: void
	 *
	 * Attributes: abstract
	 */
	virtual void OnLoseFocus()
	{
		element->OnLoseFocus();
	}

	/**
	 * Method: TTextIntercepter::OnCopy
	 * Purpose: Tells the target that CTRL-C was pressed
	 * Parameters: void
	 * Returns: void
	 *
	 * Attributes: abstract
	 */
	virtual void OnCopy()
	{
		element->OnCutCopyPaste(control_text_mode::ctm_copy);
	}
	/**
	 * Method: TTextIntercepter::OnCut
	 * Purpose: Tells the target that CTRL-X was pressed
	 * Parameters: void
	 * Returns: void
	 *
	 * Attributes: override
	 */
	virtual void OnCut()
	{
		element->OnCutCopyPaste(control_text_mode::ctm_cut);
	}

	/**
	 * Method: TTextElementIntercepter::GetTarget
	 * Purpose: Reports the TTextElement address it points to
	 * Parameters: void
	 * Returns: void* - address of the TTextElement
	 *
	 * Attributes: override
	 */
	virtual void* GetTarget() override
	{
		return element.Get();
	}


	/**
	 * Method: TTextInterceptor::TakesInput
	 * Purpose: Reports whather input will be added to the target
	 * Parameters: void
	 * Returns: bool - whether the target supports input or not
	 */
	virtual bool TakesInput() override
	{
		return element->TakesInput();
	}

	TTextElementIntercepter(TrecPointer<TTextElement> element)
	{
		this->element = element;
		assert(element.Get());
	}
private:
	TrecPointer<TTextElement> element;
};

void TTextElement::SetSelf(TrecPointer<TTextElement> e)
{
	if (this != e.Get())
		throw 1;
	self = TrecPointerKey::SoftFromTrec<>(e);
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

bool TTextElement::GetTextFormattingDetails(TextFormattingDetails& details, UINT location)
{
	details = this->formattingDetails;
	return true;
}

void TTextElement::AppendLine(BasicCharLine& curLine, float& y)
{
	curLine.totalWidth = 0.0f;
	for (UINT C = 0; C < curLine.characters.Size(); C++)
	{
		BasicCharacter tempChar(curLine.characters[C]);
		curLine.height = std::max(curLine.height, tempChar.location.bottom - tempChar.location.top);
		curLine.totalWidth += (tempChar.location.right - tempChar.location.left);
	}

	for (UINT C = 0; C < curLine.characters.Size(); C++)
	{
		auto& curChar = curLine.characters[C];
		float heightDif = curLine.height - (curChar.location.bottom - curChar.location.top);

		curChar.location.bottom += heightDif;
		curChar.location.top += heightDif;
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

TrecPointer<TTextIntercepter> TTextElement::GetTextInterceptor()
{
	if (!interceptor.Get())
	{
		auto fullSelf = TrecPointerKey::TrecFromSoft<>(self);
		interceptor = TrecPointerKey::GetNewTrecPointerAlt<TTextIntercepter, TTextElementIntercepter>(fullSelf);
	}

	return interceptor;
}

TrecPointer<TStringVariable> TTextElement::GetText()
{
	return this->text;
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
	assert(InitializeText());

	wrap = true;
	isClickDown = false;
	bounds.bottom = bounds.left = bounds.right = bounds.top = 0.0f;

	text = TrecPointerKey::ConvertPointer<TVariable, TStringVariable>(
		TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(L""));

	VBO = VAO = 0;
}

bool TTextElement::InitializeText()
{
	if (freeTypeLibrary)
		return true;
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
		//drawingBoard->GetDisplayResolution(width, height);
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
	glfwMakeContextCurrent(drawingBoard->GetGlfwWindow());

	if (VAO)
	{
		glDeleteVertexArrays(1, &VAO);
		VAO = 0;
	}
	glGenVertexArrays(1, &VAO);
	if (VBO)
	{
		glDeleteBuffers(1, &VBO);
		VBO = 0;
	}
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

	lines.RemoveAll();

	BasicCharLine curLine;

	const TString& theText = text->GetString();

	FT_Face curFace = nullptr;

	int width = 300;
	int height = 300;
	//drawingBoard->GetDisplayResolution(width, height);
	//FT_Set_Char_Size(curFace, 0, 16 * 64, width, height);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

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
			x = bounds.left;
		}
		if (ch.character == L'\n')
		{
			if (prevChar != L'\r')
			{
				curLine.strIndex = startIndex;
				startIndex = Rust;
				AppendLine(curLine, y);
				x = bounds.left;
			}
		}

		prevChar = ch.character;

		TextFormattingDetails chFormatting;
		assert(this->GetTextFormattingDetails(chFormatting, Rust));
		if (!RetrieveFont(chFormatting.font, curFace))
			return;

		FT_Set_Pixel_Sizes(curFace, 0, chFormatting.fontSize);

		FT_UInt glyphIndex = FT_Get_Char_Index(curFace, ch.character);
		if(!glyphIndex)continue;

		if (FT_Load_Glyph(curFace, glyphIndex, FT_LOAD_COLOR))
			continue;

		if(FT_Render_Glyph(curFace->glyph, FT_RENDER_MODE_NORMAL))continue;


		ch.location.top = y;
		ch.location.bottom = y + curFace->glyph->bitmap.rows;

		ch.advanceY = curFace->glyph->advance.y;

		float under = curFace->glyph->bitmap.rows - curFace->glyph->bitmap_top;
		ch.location.top += under;
		ch.location.bottom += under;

		//float under = (static_cast<float>(curFace->glyph->metrics.height) - static_cast<float>(curFace->glyph->metrics.vertBearingY)) / static_cast<float>(curFace->glyph->metrics.height);

		//under = (ch.location.bottom - ch.location.top) * under;

		//ch.location.bottom -= under;
		//ch.location.top -= under;

		float advanceX = static_cast<float>(curFace->glyph->metrics.horiAdvance) / static_cast<float>(curFace->glyph->metrics.width);

		ch.location.left = x;
		ch.location.right = x + curFace->glyph->bitmap.width;

		// Since a space yields 0 space, use 'o' as a dummy face to aritficially shift the x
		if (!curFace->glyph->bitmap.width)
		{
			FT_Face tempFace = nullptr;
			assert(RetrieveFont(chFormatting.font, tempFace));
			FT_Set_Pixel_Sizes(curFace, 0, chFormatting.fontSize);
			FT_UInt tempGlyphIndex = FT_Get_Char_Index(tempFace, L'o');


			assert(tempGlyphIndex && !FT_Load_Glyph(tempFace, tempGlyphIndex, FT_LOAD_COLOR) && !FT_Render_Glyph(tempFace->glyph, FT_RENDER_MODE_NORMAL));
			x += tempFace->glyph->bitmap.width;
			//continue;
		}
		else
			x += (curFace->glyph->bitmap.width * advanceX);

		ch.backgroundColor = chFormatting.defaultBackgroundColor;

		GLuint texId = 0;
		if (!GetFontCharacter(chFormatting.font, ch.character, ch.GetWeightStrength(), texId, drawingBoard))
		{
			glGenTextures(1, &texId);
			glBindTexture(GL_TEXTURE_2D, texId);
			glTexImage2D(GL_TEXTURE_2D,
				0,
				GL_RED,
				curFace->glyph->bitmap.width,
				curFace->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				curFace->glyph->bitmap.buffer);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			SetFontCharacter(chFormatting.font, ch.character, ch.GetWeightStrength(), texId, drawingBoard);
		}
		
		curLine.characters.push_back(ch);

		if (x > bounds.right)
		{
			if (wrap)
			{
				curLine.strIndex = startIndex;
				startIndex = Rust;
				AppendLine(curLine, y);
				x = bounds.left;
				curLine.isCarryOver = true;
			}
		}
	}

	if (!theText.EndsWith(L'\n'))
	{
		curLine.strIndex = startIndex;
		AppendLine(curLine, y);
		x = bounds.left;
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

		for (UINT C = 0; C < lines[Rust].characters.Size(); C++)
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
	
	
	RECT_F proxy = { 0,0,0,0 };
	

	// First, Draw the Backgrounds or highlights
	for (UINT Rust = 0; Rust < lines.Size(); Rust++)
	{
		BasicCharLine& line = lines[Rust];
		for (UINT C = 0; C < line.characters.Size(); C++)
		{
			BasicCharacter& ch = line.characters[C];
			assert(TColorBrush::NormalizeRect(proxy, ch.location, this->drawingBoard));
			if (ch.isHighlighted)
			{
				auto brush = drawingBoard->GetHighlightBrush();
				auto solidBrush = TrecPointerKey::ConvertPointer<TBrush, TColorBrush>(brush);
				solidBrush->FillRectangle(proxy);
			}
			else if (ch.backgroundColor.Get())
			{
				auto brush = drawingBoard->GetSolidColorBrush(*ch.backgroundColor.Get());
				auto solidBrush = TrecPointerKey::ConvertPointer<TBrush, TColorBrush>(brush);
				solidBrush->FillRectangle(proxy);
			}

		}
	}

	GLint loc = glGetUniformLocation(drawingBoard->SetShader(TrecPointer<TShader>(), shader_type::shader_write), "textColor");


	bool cullDisabled = !glIsEnabled(GL_CULL_FACE);
	bool blendDisabled = !glIsEnabled(GL_BLEND);

	GLint blendSrc;
	GLint blendDst;
	glGetIntegerv(GL_BLEND_SRC_ALPHA, &blendSrc);
	glGetIntegerv(GL_BLEND_DST_ALPHA, &blendDst);

	if (cullDisabled)
		glEnable(GL_CULL_FACE);
	if (blendDisabled)
		glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	assert(TImageBrush::NormalizeRect(proxy, bounds, drawingBoard));

	for (UINT Rust = 0; Rust < lines.Size(); Rust++)
	{
		BasicCharLine& line = lines[Rust];
		for (UINT C = 0; C < line.characters.Size(); C++)
		{
			BasicCharacter& ch = line.characters[C];

			RECT_F chProxy = { 0,0,0,0 };
			assert(TImageBrush::NormalizeRect(chProxy, ch.location, drawingBoard));

			float* verticies = TImageBrush::GeneratePictureVertices(chProxy, proxy);
			if (!verticies)
				continue;

			TextFormattingDetails chFormatting;
			assert(this->GetTextFormattingDetails(chFormatting, C + line.strIndex));

			GLuint texId = 0;
			if (!GetFontCharacter(chFormatting.font, ch.character, ch.GetWeightStrength(), texId, drawingBoard))
				continue;

			glUniform4f(
				loc
				, chFormatting.defaultTextColor.GetRed()
				, chFormatting.defaultTextColor.GetGreen()
				, chFormatting.defaultTextColor.GetBlue()
				, chFormatting.defaultTextColor.GetOpacity()
			);

			glBindTexture(GL_TEXTURE_2D, texId);
			// update content of VBO memory
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float[24]), verticies); // be sure to use glBufferSubData and not glBufferData

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			// render quad
			glDrawArrays(GL_TRIANGLES, 0, 6);
			delete[] verticies;
			verticies = nullptr;
		}
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (cullDisabled)
		glDisable(GL_CULL_FACE);
	if (blendDisabled)
		glDisable(GL_BLEND);

	glBlendFunc(blendSrc, blendDst);
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
		auto& line = lines[Rust];
		float curWidth = line.isCarryOver ? width : 0;

		for (UINT C = 0; C < line.characters.Size(); C++)
		{
			auto& ch = line.characters[C];
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

void TTextElement::SetBasicFormatting(TextFormattingDetails& details)
{
	this->formattingDetails = details;
}
void TTextElement::SetHorizontalAllignment(tc_line_spacing s)
{
	formattingDetails.defaultLineSpacing = s;
}
void TTextElement::SetVerticalAllignment(tc_text_spacing s)
{
	formattingDetails.textSpacing = s;
}


void TTextElement::GetLineMetrics(TDataArray<LineMetrics>& metricsList)
{
	metricsList.RemoveAll();

	for (UINT Rust = 0; Rust < lines.Size(); Rust++)
	{
		LineMetrics mets;
		BasicCharLine& line = lines[Rust];

		mets.isCarryOver = line.isCarryOver;
		mets.ceilingPadding = line.ceilingPadding;
		mets.charCount = line.characters.Size();
		mets.floorPadding = line.floorPadding;
		mets.height = line.height;
		mets.top = line.top;

		mets.carriesOver = false;

		if (mets.isCarryOver && Rust)
			metricsList[Rust - 1].carriesOver = true;
		metricsList.push_back(mets);
	}
}

TextFormattingDetails::TextFormattingDetails():
	fontSize(16.0f),
	formatTweaks(0),
	italicValue(0),
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
	strIndex = 0;
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
	return std::max(0.0f, isFloor ? floorPadding : ceilingPadding);
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
	advanceY = 0.0f;
	//FT_Bitmap_Init(&bitmap);
}

BasicCharacter::BasicCharacter(const BasicCharacter& copy)
{
	backgroundColor = copy.backgroundColor;
	character = copy.character;
	format = copy.format;
	isHighlighted = copy.isHighlighted;
	location = copy.location;
	advanceY = copy.advanceY;
}

BasicCharacter::~BasicCharacter()
{
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
    return true;
}
