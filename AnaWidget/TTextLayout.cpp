#include "TTextLayout.h"

/*
* Function: operator==
* Purpose: Compares two colors
* Parameters: D2D1_COLOR_F & c1 - the first color to compare
*				D2D1_COLOR_F & c2 - the second color to compare
* Returns: bool - whether the values are equal
*/
//bool operator==(D2D1_COLOR_F& c1, D2D1_COLOR_F& c2)
//{
//	return c1.r == c2.r && c1.g == c2.b
//		&& c1.b == c2.b && c1.a == c2.a;
//}


void TTextLayout::SetUpTextElement()
{
	TextFormattingDetails details;

	TString locale(L"en-us"), font(L"ariel");

	TString actText;

	tc_line_spacing hAlign = tc_line_spacing::center;
	tc_text_spacing vAlign = tc_text_spacing::center;

	TString valpoint;

	TColor fontColor;
	if (attributes.retrieveEntry(TString(L"Caption"), valpoint))
	{
		actText.Set(valpoint);
	}

	if (attributes.retrieveEntry(TString(L"CaptionLocale"), valpoint))
	{
		locale.Set(valpoint);
	}

	if (attributes.retrieveEntry(TString(L"Font"), valpoint))
	{
		font.Set(valpoint);
	}
	if (attributes.retrieveEntry(TString(L"FontSize"), valpoint))
	{
		valpoint.ConvertToFloat(details.fontSize);
	}
	if (attributes.retrieveEntry(TString(L"HorizontalAlignment"), valpoint))
	{
		hAlign = convertStringToTextAlignment(valpoint);
	}
	if (attributes.retrieveEntry(TString(L"VerticalAlignment"), valpoint))
	{
		vAlign = convertStringToParagraphAlignment(valpoint);
	}

	if (attributes.retrieveEntry(TString(L"FontColor"), valpoint))
	{
		bool b = false;
		fontColor.SetColor(TColor::GetColorFromString(valpoint, b));
	}
	else
		fontColor.SetColor("black");

	// Commented out code (in case gradient colors should be added

	//valpoint = att->retrieveEntry(TString(L"TextGrad"));
	//if (valpoint.Get())
	//{
	//	if (!text1.Get())
	//		text1 = TrecPointerKey::GetNewTrecPointer<TText>(drawingBoard, (this));
	//	float entry = 0.0f;
	//	valpoint->ConvertToFloat(entry);
	//	//text1->secondColor = true;
	//	UINT gradCount = 0;
	//	if (gradCount = text1->stopCollection.GetGradientCount())
	//		text1->stopCollection.SetPositionAt(entry, gradCount - 1);
	//}


	//valpoint = att->retrieveEntry(TString(L"TextGradMode"));
	//if (valpoint.Get())
	//{
	//	if (!text1.Get())
	//		text1 = TrecPointerKey::GetNewTrecPointer<TText>(drawingBoard, (this));
	//	if (valpoint->Compare(L"Radial"))
	//		text1->useRadial = true;
	//}

	// See if have text. If we do, then set up text element

	details.defaultTextColor = fontColor;

	text = TrecPointerKey::GetNewSelfTrecPointerAlt<TTextElement, TComplexTextElement>(drawingBoard, windowHandle);

	text->SetBounds(area);
	text->SetBasicFormatting(details);
	text->SetHorizontalAllignment(hAlign);
	text->SetVerticalAllignment(vAlign);
	text->SetText(actText);
}

UINT TTextLayout::AppendText(const TString& str)
{
	// To Do: Add means of deleting characters at the beggining if a limit was set
	TString t;

	// End to to
	this->text->GetText(t);
	this->text->SetText(t + str);

	return 0;
}

void TTextLayout::AddFontSize(float size, UINT start, UINT length)
{
	TObjectLocker threadLock(&thread);
	if (!text.Get() || !drawingBoard.Get())
	{
		return;
	}

	auto cText = TrecPointerKey::ConvertPointer<TTextElement, TComplexTextElement>(text);
	assert(cText.Get());

	TrecPointer<FormatChangeFontSize> colorEffect = TrecPointerKey::GetNewTrecPointer<FormatChangeFontSize>();
	colorEffect->fontSize = size;
	colorEffect->textStart = start;
	colorEffect->textEnd = start + length;

	cText->SetFormatting(TrecPointerKey::ConvertPointer<FormatChangeFontSize, FormatChange>(colorEffect), start, start + length);
}

void TTextLayout::AddFontWeight(UCHAR weight, UINT start, UINT length)
{
	TObjectLocker threadLock(&thread);
	if (!text.Get() || !drawingBoard.Get())
	{
		return;
	}

	auto cText = TrecPointerKey::ConvertPointer<TTextElement, TComplexTextElement>(text);
	assert(cText.Get());

	TrecPointer<FormatChangeFontWeight> colorEffect = TrecPointerKey::GetNewTrecPointer<FormatChangeFontWeight>();
	colorEffect->tweak = weight;
	colorEffect->textStart = start;
	colorEffect->textEnd = start + length;

	cText->SetFormatting(TrecPointerKey::ConvertPointer<FormatChangeFontWeight, FormatChange>(colorEffect), start, start + length);
}

void TTextLayout::AddFont(const TString& font, UINT start, UINT length)
{
	TObjectLocker threadLock(&thread);
	if (!text.Get() || !drawingBoard.Get())
	{
		return;
	}

	auto cText = TrecPointerKey::ConvertPointer<TTextElement, TComplexTextElement>(text);
	assert(cText.Get());

	TrecPointer<FormatChangeFont> colorEffect = TrecPointerKey::GetNewTrecPointer<FormatChangeFont>();
	colorEffect->font.Set(font);
	colorEffect->textStart = start;
	colorEffect->textEnd = start + length;

	cText->SetFormatting(TrecPointerKey::ConvertPointer<FormatChangeFont, FormatChange>(colorEffect), start, start + length);
}

void TTextLayout::AddBackgroundColor(const TString& col, UINT start, UINT length)
{
	TObjectLocker threadLock(&thread);
	if (!text.Get() || !drawingBoard.Get())
	{
		return;
	}

	auto cText = TrecPointerKey::ConvertPointer<TTextElement, TComplexTextElement>(text);
	assert(cText.Get());

	TrecPointer<FormatChangeBackgroundColor> colorEffect = TrecPointerKey::GetNewTrecPointer<FormatChangeBackgroundColor>();
	colorEffect->color = col;
	colorEffect->textStart = start;
	colorEffect->textEnd = start + length;

	cText->SetFormatting(TrecPointerKey::ConvertPointer<FormatChangeBackgroundColor, FormatChange>(colorEffect), start, start + length);
}

void TTextLayout::AddColorEffect(TColor col, UINT start, UINT length)
{
	TObjectLocker threadLock(&thread);
	if (!text.Get() || !drawingBoard.Get())
	{
		return;
	}

	auto cText = TrecPointerKey::ConvertPointer<TTextElement, TComplexTextElement>(text);
	assert(cText.Get());

	TrecPointer<FormatChangeColor> colorEffect = TrecPointerKey::GetNewTrecPointer<FormatChangeColor>();
	colorEffect->color = col;
	colorEffect->textStart = start;
	colorEffect->textEnd = start + length;

	cText->SetFormatting(TrecPointerKey::ConvertPointer<FormatChangeColor, FormatChange>(colorEffect), start, start + length);
}

TrecPointer<BasicCharLine> TTextLayout::GetLineMetrics()
{
	//TObjectLocker threadLock(&thread);
	//if (text.Get() && text->GetLayout().Get())
	//{
	//	TrecPointer<LineMetrics2> ret = TrecPointerKey::GetNewTrecPointer<LineMetrics2>(5);
	//	HRESULT res = text->GetLayout()->GetLineMetrics(ret->metrics.data(), ret->metrics.Size(), &ret->sizeNeeded);
	//	if (res == E_NOT_SUFFICIENT_BUFFER)
	//	{
	//		ret->SetSize(ret->sizeNeeded);
	//		text->GetLayout()->GetLineMetrics(ret->metrics.data(), ret->metrics.Size(), &ret->sizeNeeded);

	//		return ret;
	//	}
	//}

	return TrecPointer<BasicCharLine>();
}



void TTextLayout::ShrinkHeight()
{
	if (text.Get())
	{
		float height = 0.0f;
		text->GetMinHeight(height);
		area.bottom = area.top + height;
		text->SetBounds(area);
	}
}

TTextLayout::TTextLayout(TrecPointer<DrawingBoard> rt, TDataMap<TDataMap<TString>> ta, HWND win) : TTextInput(rt, ta, win)
{
}

TTextLayout::~TTextLayout()
{
}

bool TTextLayout::onCreate(const RECT_F& loc, TrecPointer<TFileShell> d)
{
	TString valpoint;
	if (attributes.retrieveEntry(L"IsLocked", valpoint) && !valpoint.CompareNoCase(L"true"))
	{
		this->textLock = true;
	}
	return TGadget::onCreate(loc, d);
}

