#pragma once
#include "TInputTextElement.h"

class _TC_GRAPH FormatChange: public TCoreObject
{
public:
	FormatChange() = default;
	FormatChange(const FormatChange& copy) = default;
	UINT textStart; 
	UINT textEnd;
	virtual UINT GetType() = 0;
	virtual bool IsTraitEqual(FormatChange& copy) = 0;
};

class _TC_GRAPH FormatChangeFontSize : public FormatChange
{
public:
	FormatChangeFontSize() = default;
	FormatChangeFontSize(const FormatChangeFontSize& copy) = default;
	float fontSize;
	UINT GetType() override;
	bool IsTraitEqual(FormatChange& copy) override;
};

class _TC_GRAPH FormatChangeFont : public FormatChange
{
public:
	FormatChangeFont() = default;
	FormatChangeFont(const FormatChangeFont& copy) = default;
	TString font;
	UINT GetType() override;
	bool IsTraitEqual(FormatChange& copy) override;
};

class _TC_GRAPH FormatChangeFontWeight : public FormatChange
{
public:
	FormatChangeFontWeight() = default;
	FormatChangeFontWeight(const FormatChangeFontWeight& copy) = default;
	UCHAR tweak;
	UINT GetType() override;
	bool IsTraitEqual(FormatChange& copy) override;
};

class _TC_GRAPH FormatChangeFontSlant : public FormatChange
{
public:
	FormatChangeFontSlant() = default;
	FormatChangeFontSlant(const FormatChangeFontSlant& copy) = default;
	UCHAR tweak;
	UINT GetType() override;
	bool IsTraitEqual(FormatChange& copy) override;
};

class _TC_GRAPH FormatChangeColor : public FormatChange
{
public:
	FormatChangeColor() = default;
	FormatChangeColor(const FormatChangeColor& copy) = default;
	TColor color;
	UINT GetType() override;
	bool IsTraitEqual(FormatChange& copy) override;
};

class _TC_GRAPH FormatChangeBackgroundColor : public FormatChange
{
public:
	FormatChangeBackgroundColor() = default;
	FormatChangeBackgroundColor(const FormatChangeBackgroundColor& copy) = default;
	TColor color;
	UINT GetType() override;
	bool IsTraitEqual(FormatChange& copy) override;
};

/**
 * Class: TComplexTextElement
 * Purpose: Extends the Input Text Element to provide support for complex formats
 */
class _TC_GRAPH TComplexTextElement : public TInputTextElement
{
protected:

	TDataArray<FormatChangeFontSize> sizeFormatting;
	TDataArray<FormatChangeFontWeight> weightFormatting;
	TDataArray<FormatChangeFontSlant> slantFormatting;
	TDataArray<FormatChangeFont> fontFormatting;
	TDataArray<FormatChangeColor> colorFormatting;
	TDataArray<FormatChangeBackgroundColor> backgroundColorFormatting;

	bool GetTextFormattingDetails(TextFormattingDetails& details, UINT location) override;


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
	//virtual void ReCreateLayout();

public:

	virtual bool SetText(const TString& text);

	TComplexTextElement(TrecPointer<DrawingBoard> drawingBoard, bool editAllowed = true);

	virtual bool SetFormatting(TrecPointer<FormatChange> change, UINT start, UINT end);

	virtual bool OnCLickDown(const TPoint& point) override;

	virtual bool OnCLickUp(const TPoint& point) override;

	virtual bool OnMouseMove(const TPoint& point) override;

	virtual void OnCutCopyPaste(control_text_mode mode) override;

	virtual bool OnInputChar(WCHAR ch, UINT count) override;

	virtual void OnTransferText(UINT newPos);
};

