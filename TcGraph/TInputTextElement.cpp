#include "TInputTextElement.h"
#include "TColorBrush.h"

TInputTextElement::TInputTextElement(TrecPointer<DrawingBoard> board): TTextElement(board)
{
	carotLoc = 0;
	carotActive = grabbedRange = false;
	editAllowed = true;
}

bool TInputTextElement::FindString(const TString& target, UINT& index, bool fromFront)
{
	if (!text.Get())
		return false;
	auto& strText = text->GetString();
	int iindex = fromFront ? strText.Find(target, 0) : strText.FindLast(target);
	if (iindex == -1)
		return false;
	index = static_cast<UINT>(iindex);

	return true;
}

void TInputTextElement::UpdateCarotPoisition(UINT loc)
{
}

bool TInputTextElement::GetCarotLoc(UINT& loc)
{
	return false;
}

void TInputTextElement::OnDraw(TrecPointer<TVariable> dataText)
{
	TTextElement::OnDraw(dataText);

	if (carotActive && static_cast<ULONG64>(glfwGetTime()) % 2)
	{
		// Draw the Carot
		TrecPointer<TBrush> carotBrush = drawingBoard->GetSolidColorBrush(TColor());
		TrecPointer<TColorBrush> colorBrush = TrecPointerKey::ConvertPointer<TBrush, TColorBrush>(carotBrush);
	
		for (UINT Rust = 0; Rust < lines.Size(); Rust++)
		{
			auto& line = lines[Rust];
			if (Rust == line.strIndex && line.characters.Size() == 0)
			{
				TPoint topPoint, bottomPoint;

				topPoint.y = line.top;
				bottomPoint.y = line.top + line.height;

				switch (formattingDetails.defaultLineSpacing)
				{
				case tc_line_spacing::justified:
				case tc_line_spacing::left:
					topPoint.x = bottomPoint.x = bounds.left;
					break;
				case tc_line_spacing::right:
					topPoint.x = bottomPoint.x = bounds.right;
					break;
				case tc_line_spacing::center:
					topPoint.x = bottomPoint.x = bounds.left + ((bounds.right - bounds.left) / 2);
				}

				colorBrush->DrawLine(TPoint( topPoint.x, topPoint.y ), TPoint( bottomPoint.x, bottomPoint.y ));
				return;
			}

			if (Rust >= line.strIndex && Rust < (line.strIndex + line.characters.Size()))
			{
				auto& ch = line.characters[Rust - line.strIndex];
				TPoint topPoint, bottomPoint;
				topPoint.y = ch.location.top;
				bottomPoint.y = ch.location.bottom;
				topPoint.x = bottomPoint.x = ch.location.left;
				colorBrush->DrawLine(TPoint( topPoint.x, topPoint.y ), TPoint( bottomPoint.x, bottomPoint.y ));
				return;
			}
		}
	}
}

bool TInputTextElement::OnCLickDown(const TPoint& point)
{
	if (DrawingBoard::IsContained(point, bounds))
	{
		UINT mets = 0;
		BOOL trailing = FALSE, inside = FALSE;
		if (HitTestPoint(point, trailing, inside, mets))
		{
			UINT pos = trailing ? 1 : 0;
			pos += mets;


			isClickDown = true;

			// Check to see if we just grabbed onto highlighted text
			UINT curStart = 0, curEnd = 0;
			if (this->highlightRange.GetHighlightRange(curStart, curEnd) && curStart <= pos && pos <= curEnd && curStart != curEnd)
				this->grabbedRange = true;
			else
			{
				this->highlightRange.Reset();
				this->highlightRange.SetBegin(pos);
			}
		}
		return true;
	}
	return false;
}

bool TInputTextElement::OnCLickUp(const TPoint& point)
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
			if (grabbedRange)
			{
				OnTransferText(pos);
				grabbedRange = false;
			}

			ClearHighlight();

			// Range Highlighter
			if (this->highlightRange.GetHighlightRange(start, end) && start < end)
			{
				this->SetHighlight(start, end);
			}
			this->highlightRange.SetEnd(pos);

			
			carotActive = this->highlightRange.GetCarotLocation(this->carotLoc);
			

		}

		return true;
	}
	return false;
}

bool TInputTextElement::OnMouseMove(const TPoint& point)
{
	if (DrawingBoard::IsContained(point, bounds))
	{
		UINT mets = 0;
		BOOL trailing = FALSE, inside = FALSE;
		if (isClickDown && HitTestPoint(point, trailing, inside, mets))
		{
			UINT pos = trailing ? 1 : 0;
			pos += mets;
			if (grabbedRange)
			{
				UpdateCarotPoisition(pos);
			}
			else
			{

				UINT start = 0, end = 0;
				ClearHighlight();
				this->highlightRange.SetEnd(pos);

				// Range Highlighter
				if (this->highlightRange.GetHighlightRange(start, end))
				{
					SetHighlight(start, end);
				}
			}
		}
		return true;

	}
	return false;
}

void TInputTextElement::OnCutCopyPaste(control_text_mode mode)
{
}

bool TInputTextElement::OnInputChar(WCHAR ch, UINT count)
{
	UINT start = 0, end = 0;
	if (carotActive && editAllowed)
	{
		bool didDelete = false;
		auto& strText = text->GetString();
		if (this->highlightRange.GetHighlightRange(start, end))
		{
			strText.Delete(start, end - start);
			ClearHighlight();
			highlightRange.Reset();
			didDelete = true;
			carotLoc = start;
		}
		for (int c = 0; c < count; c++)
		{
			switch (ch)
			{
			case 0x08: // Back
				if (carotLoc && !didDelete)
					strText.Delete(--carotLoc);
				break;
			case 0xBE: // OEM-Period
				strText.Insert(carotLoc++, L'.');
				break;
			case 0x6D: // Subtract
				if (ch == L'm')
					goto def;
				strText.Insert(carotLoc++, L'-');
				break;
			case 0x6F: // Divide
				if (ch == L'o')
					goto def;
				strText.Insert(carotLoc++, L'/');
				break;
			default:
			def:
				strText.Insert(carotLoc++, ch);
			}
		}
		ReCreateLayout();
		UpdateCarotPoisition(carotLoc);
	}
	return true;
}

bool TInputTextElement::LockText(bool doLock)
{
	return false;
}

bool TInputTextElement::TakesInput()
{
	return false;
}

bool TInputTextElement::CanTakeInput()
{
	return false;
}

void TInputTextElement::OnLoseFocus()
{
	carotActive = false;
}

void TInputTextElement::OnTransferText(UINT newPos)
{
	UINT start = 0, end = 0;

	if (!highlightRange.GetHighlightRange(start, end) || start == end)
		return;

	if (start <= newPos && newPos <= end)
		return;

	auto& strText = text->GetString();

	TString subRange(strText.SubString(start, end));

	ClearHighlight();

	if (newPos < start)
	{
		strText.Delete(start, end - start);
		strText.Insert(newPos, subRange);
		start = newPos;
		end = start + subRange.GetSize();
	}
	else
	{
		strText.Insert(newPos, subRange);
		strText.Delete(start, end - start);
		start = newPos - subRange.GetSize();
		end = start + subRange.GetSize();
	}

	highlightRange.Reset();
	highlightRange.SetBegin(start);
	highlightRange.SetEnd(end);

	ReCreateLayout();

	SetHighlight(start, end);
}
