#pragma once
#include "TInputTextElement.h"

/**
 * Class: TComplexTextElement
 * Purpose: Extends the Input Text Element to provide support for complex formats
 */
class _TC_GRAPH TComplexTextElement : public TInputTextElement
{
protected:

	/**
	 * Formatting Details
	 */
	TDataArray<TextFormattingDetails> formatting;


	/*
	 * Method: TComplexTextElement::ReCreateLayout
	 * Purpose: Resets the Text Layout upon changes made to the format, text, or location
	 * Parameters: void
	 * Returns: void
	 * Note: Call this method whenever you make a change to the format of the control, the text, or the location,
	 *		Otherwise, your changes might not be seen
	 *
	 * Attributes: virtual
	 */
	virtual void ReCreateLayout();

public:

	virtual bool SetText(const TString& text);

	TComplexTextElement(TrecPointer<DrawingBoard> drawingBoard, bool editAllowed = true);

	virtual bool SetFormatting(const TextFormattingDetails& details, UINT start, UINT end, bool correctInvalidParams);

	virtual bool OnCLickDown(const TPoint& point) override;

	virtual bool OnCLickUp(const TPoint& point) override;

	virtual bool OnMouseMove(const TPoint& point) override;

	virtual void OnCutCopyPaste(control_text_mode mode) override;

	virtual bool OnInputChar(WCHAR ch, UINT count) override;

	virtual void OnTransferText(UINT newPos);
};

