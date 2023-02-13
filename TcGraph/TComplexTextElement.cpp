#include "TComplexTextElement.h"

bool TComplexTextElement::GetTextFormattingDetails(TextFormattingDetails& details, UINT location)
{
	if(!text.Get() || text->GetString().GetSize() <= location)
		return false;
	details = this->formattingDetails;

	for (UINT Rust = 0; Rust < sizeFormatting.Size(); Rust++)
	{
		auto& temp = sizeFormatting[Rust];
		if (location >= temp.textStart && location < (temp.textEnd))
		{
			details.fontSize = temp.fontSize;
			break;
		}
	}
	for (UINT Rust = 0; Rust < weightFormatting.Size(); Rust++)
	{
		auto& temp = weightFormatting[Rust];
		if (location >= temp.textStart && location < (temp.textEnd))
		{
			details.formatTweaks = temp.tweak;
			break;
		}
	}
	for (UINT Rust = 0; Rust < slantFormatting.Size(); Rust++)
	{
		auto& temp = slantFormatting[Rust];
		if (location >= temp.textStart && location < (temp.textEnd))
		{
			details.italicValue = temp.tweak;
			break;
		}
	}
	for (UINT Rust = 0; Rust < fontFormatting.Size(); Rust++)
	{
		auto& temp = fontFormatting[Rust];
		if (location >= temp.textStart && location < (temp.textEnd))
		{
			details.font.Set(temp.font);
			break;
		}
	}
	for (UINT Rust = 0; Rust < colorFormatting.Size(); Rust++)
	{
		auto& temp = colorFormatting[Rust];
		if (location >= temp.textStart && location < (temp.textEnd))
		{
			details.defaultTextColor = temp.color;
			break;
		}
	}
	for (UINT Rust = 0; Rust < backgroundColorFormatting.Size(); Rust++)
	{
		auto& temp = backgroundColorFormatting[Rust];
		if (location >= temp.textStart && location < (temp.textEnd))
		{
			details.defaultBackgroundColor = TrecPointerKey::GetNewTrecPointer<TColor>(temp.color);
			break;
		}
	}
	return true;
}


bool TComplexTextElement::SetText(const TString& text)
{
	if (editAllowed)
		return false;

	sizeFormatting.RemoveAll();
	weightFormatting.RemoveAll();
	slantFormatting.RemoveAll();
	fontFormatting.RemoveAll();
	colorFormatting.RemoveAll();
	backgroundColorFormatting.RemoveAll();
	return TTextElement::SetText(text);
}

TComplexTextElement::TComplexTextElement(TrecPointer<DrawingBoard> drawingBoard, bool editAllowed) : TInputTextElement(drawingBoard)
{
	this->editAllowed = editAllowed;
}


template<class T> void AppendFormatting(TDataArray<T>& formatList, T& fc)
{
	if (!formatList.Size())
	{
		formatList.push_back(fc);
		return;
	}
	bool inserted = false;

	if (fc.textStart < formatList[0].textStart)
	{
		formatList.InsertAt(fc, 0);
		inserted = true;
	}
	else for (UINT Rust = 1; !inserted && Rust < formatList.Size(); Rust++)
	{
		auto& ele1 = formatList[Rust - 1];
		auto& ele2 = formatList[Rust];


		if (ele1.textStart == fc.textStart)
		{
			if (ele1.IsTraitEqual(fc) || fc.textEnd > ele1.textEnd)
			{
				formatList[Rust - 1] = fc;
				inserted = true;
			}
			else
			{
				ele1.textStart = fc.textEnd;
				formatList.InsertAt(fc, Rust - 1);
				inserted = true;
			}
		}
		else if (ele1.textStart < fc.textStart && fc.textStart < ele2.textStart)
		{
			formatList.InsertAt(fc, Rust);
			inserted = true;
		}
		else if (fc.textStart == ele2.textStart)
		{
			if (ele2.IsTraitEqual(fc) || fc.textEnd > ele2.textEnd)
			{
				formatList[Rust] = fc;
				inserted = true;
			}
			else
			{
				ele2.textStart = fc.textEnd;
				formatList.InsertAt(fc, Rust);
				inserted = true;
			}
		}
	}

	if (!inserted)
		formatList.push_back(fc);

	for (UINT Rust = 1; Rust < formatList.Size(); Rust++)
	{
		auto& ele1 = formatList[Rust - 1];
		auto& ele2 = formatList[Rust];
		if (ele1.textStart >= ele1.textEnd)
		{
			formatList.RemoveAt(--Rust);
			continue;
		}
		if ((ele2.textStart >= ele2.textEnd) || (ele1.textEnd >= ele2.textEnd))
		{
			formatList.RemoveAt(Rust--);
			continue;
		}

		if (ele1.textEnd < ele2.textStart)
			ele2.textStart = ele1.textEnd;

		if (ele1.textEnd == ele2.textStart && ele1.IsTraitEqual(ele2))
		{
			ele1.textEnd = ele2.textEnd;
			formatList.RemoveAt(Rust--);
			continue;
		}
	}
}

bool TComplexTextElement::SetFormatting(TrecPointer<FormatChange> change, UINT start, UINT end)
{
	if (!change.Get() || !text.Get() || (start >= end) || (end >= text->GetString().GetSize()))
		return false;

	UINT changeType = change->GetType();

	if (changeType == 1)
	{
		// Size
		FormatChangeFontSize* fontChange = dynamic_cast<FormatChangeFontSize*>(change.Get());

		AppendFormatting(sizeFormatting, *fontChange);
	}
	else if (changeType == 2) 
	{
		// Font
		FormatChangeFont* fontChange = dynamic_cast<FormatChangeFont*>(change.Get());
		FT_Face face;
		if (!TTextElement::RetrieveFont(fontChange->font, face))
			return false;
		AppendFormatting(fontFormatting, *fontChange);
	}
	else if (changeType == 3)
	{
		// Weight
		FormatChangeFontWeight* fontChange = dynamic_cast<FormatChangeFontWeight*>(change.Get());

		AppendFormatting(weightFormatting, *fontChange);
	}
	else if (changeType == 4)
	{
		// Italics
		FormatChangeFontSlant* fontChange = dynamic_cast<FormatChangeFontSlant*>(change.Get());

		AppendFormatting(slantFormatting, *fontChange);
	}
	else if (changeType == 5)
	{
		// color
		FormatChangeColor* fontChange = dynamic_cast<FormatChangeColor*>(change.Get());

		AppendFormatting(colorFormatting, *fontChange);
	}
	else if (changeType == 6)
	{
		// background color
		FormatChangeBackgroundColor* fontChange = dynamic_cast<FormatChangeBackgroundColor*>(change.Get());

		AppendFormatting(backgroundColorFormatting, *fontChange);
	}
	return true;
}

bool TComplexTextElement::OnCLickDown(const TPoint& point)
{
	return editAllowed ? TInputTextElement::OnCLickDown(point) : TTextElement::OnCLickDown(point);
}

bool TComplexTextElement::OnCLickUp(const TPoint& point)
{
	return editAllowed ? TInputTextElement::OnCLickUp(point) : TTextElement::OnCLickUp(point);
}

bool TComplexTextElement::OnMouseMove(const TPoint& point)
{
	return editAllowed ? TInputTextElement::OnMouseMove(point) : TTextElement::OnMouseMove(point);
}

void TComplexTextElement::OnCutCopyPaste(control_text_mode mode)
{
}

bool TComplexTextElement::OnInputChar(WCHAR ch, UINT count, UINT flags)
{
	return editAllowed ? TInputTextElement::OnInputChar(ch, count, flags) : TTextElement::OnInputChar(ch, count, flags);
}

void TComplexTextElement::OnTransferText(UINT newPos)
{
	UINT start = 0, end = 0;

	if (!highlightRange.GetHighlightRange(start, end) || start == end)
		return;

	if (start <= newPos || newPos <= end)
		return;

	TString subRange(text->GetString().SubString(start, end + 1));

	UINT formatIndex;
	//for (UINT Rust = 0; Rust < formatting.Size(); Rust++)
	//{
	//	auto f = formatting[Rust];

	//	// To-Do: Figure out how to rearange the formatting
	//}

	// If sub string uses a lot of memory, free it before parent method creates another copy

	subRange.Empty();

	//OnTransferText(newPos);
}

UINT FormatChangeFontSize::GetType()
{
	return 1;
}

bool FormatChangeFontSize::IsTraitEqual(FormatChange& copy)
{
	return abs(dynamic_cast<FormatChangeFontSize*>(&copy)->fontSize - fontSize) < 0.1f;
}

UINT FormatChangeFont::GetType()
{
	return 2;
}

bool FormatChangeFont::IsTraitEqual(FormatChange& copy)
{
	return !(dynamic_cast<FormatChangeFont*>(&copy)->font.Compare(this->font));
}

UINT FormatChangeFontWeight::GetType()
{
	return 3;
}

bool FormatChangeFontWeight::IsTraitEqual(FormatChange& copy)
{
	return dynamic_cast<FormatChangeFontWeight*>(&copy)->tweak == this->tweak;
}

UINT FormatChangeFontSlant::GetType()
{
	return 4;
}

bool FormatChangeFontSlant::IsTraitEqual(FormatChange& copy)
{
	return dynamic_cast<FormatChangeFontWeight*>(&copy)->tweak == this->tweak;
}

UINT FormatChangeColor::GetType()
{
	return 5;
}

bool FormatChangeColor::IsTraitEqual(FormatChange& copy)
{
	return dynamic_cast<FormatChangeColor*>(&copy)->color.IsEqual(color);
}

UINT FormatChangeBackgroundColor::GetType()
{
	return 6;
}

bool FormatChangeBackgroundColor::IsTraitEqual(FormatChange& copy)
{
	auto copyColor = dynamic_cast<FormatChangeBackgroundColor*>(&copy)->color;

	return copyColor.IsEqual(color);
}
