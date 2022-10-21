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

	TrecPointer<BasicCharLine> GetLineMetrics();

	bool ApplyFormatting(const TextFormattingDetails& details);

	virtual void ShrinkHeight();

	TTextLayout(TrecPointer<DrawingBoard> rt, TDataMap<TDataMap<TString>> ta, HWND win);

	virtual ~TTextLayout();

	bool onCreate(const RECT_F& loc, TrecPointer<TFileShell> d)override;
};

