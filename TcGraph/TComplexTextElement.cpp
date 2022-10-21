#include "TComplexTextElement.h"

void TComplexTextElement::ReCreateLayout()
{
	//TTextElement::ReCreateLayout();

	//for (UINT Rust = 0; Rust < formatting.Size(); Rust++)
	//{
	//	auto f = formatting[Rust];
	//	TDoubleBrushHolder* brushHolder = nullptr;

	//	if (f.color.Get())
	//		brushHolder = new TDoubleBrushHolder(f.color->GetUnderlyingBrush().Get(), f.bColor.Get() ? f.bColor->GetUnderlyingBrush().Get() : nullptr);
	//	else
	//		brushHolder = new TDoubleBrushHolder(basicDetails.color->GetUnderlyingBrush().Get(), f.bColor.Get() ? f.bColor->GetUnderlyingBrush().Get() : nullptr);

	//	if (brushHolder)
	//		mainLayout->SetDrawingEffect(brushHolder, f.range);

	//	if (f.fontSize != basicDetails.fontSize)
	//		mainLayout->SetFontSize(f.fontSize, f.range);
	//	if (f.stretch != basicDetails.stretch)
	//		mainLayout->SetFontStretch(f.stretch, f.range);
	//	if (f.style != basicDetails.style)
	//		mainLayout->SetFontStyle(f.style, f.range);
	//	if (f.weight != basicDetails.weight)
	//		mainLayout->SetFontWeight(f.weight, f.range);

	//	if (brushHolder)brushHolder->Release();
	//}
}

bool TComplexTextElement::SetText(const TString& text)
{
	if (editAllowed && formatting.Size())
		return false;

	formatting.RemoveAll();
	return TTextElement::SetText(text);
}

TComplexTextElement::TComplexTextElement(TrecPointer<DrawingBoard> drawingBoard, bool editAllowed) : TInputTextElement(drawingBoard)
{
	this->editAllowed = editAllowed;
}

bool TComplexTextElement::SetFormatting(const TextFormattingDetails& details, UINT start, UINT end, bool correctInvalidParams)
{
	if (start == end) return false;

	if (start > end)
	{
		if (!correctInvalidParams)
			return false;
		UINT temp = end;
		end = start;
		start = temp;
	}

	if (!correctInvalidParams && details.fontSize < 0.0f)
		return false;

	float fontSize = abs(details.fontSize);

	formatting.push_back(details);
	ReCreateLayout();
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

bool TComplexTextElement::OnInputChar(WCHAR ch, UINT count)
{
	return editAllowed ? TInputTextElement::OnInputChar(ch, count) : TTextElement::OnInputChar(ch, count);
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
	for (UINT Rust = 0; Rust < formatting.Size(); Rust++)
	{
		auto f = formatting[Rust];

		// To-Do: Figure out how to rearange the formatting
	}

	// If sub string uses a lot of memory, free it before parent method creates another copy

	subRange.Empty();

	OnTransferText(newPos);
}
