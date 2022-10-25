#pragma once
#include "TTextInput.h"
#include <TComplexTextElement.h>

class _ANA_WIDGET TTextLayout :
	public TTextInput
{
protected:

	virtual void SetUpTextElement() override;


	/**
	 * List for details such as weight
	 */
	TDataArray<TextFormattingDetails> details;
public:

	UINT AppendText(const TString& str);

	void AddColorEffect(TColor col, UINT start, UINT length);

	void AddFontSize(float size, UINT start, UINT length);

	void AddFontWeight(UCHAR weight, UINT start, UINT length);

	void AddFont(const TString& font, UINT start, UINT length);

	void AddBackgroundColor(const TString& col, UINT start, UINT length);

	TDataArray<LineMetrics> GetLineMetrics();

	virtual void ShrinkHeight();

	TTextLayout(TrecPointer<DrawingBoard> rt, TDataMap<TDataMap<TString>> ta);

	virtual ~TTextLayout();

	bool onCreate(const RECT_F& loc, TrecPointer<TFileShell> d)override;
};

