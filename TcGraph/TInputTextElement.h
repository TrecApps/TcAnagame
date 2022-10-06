#pragma once
#include "TTextElement.h"
class _TC_GRAPH TInputTextElement :
    public TTextElement
{
    friend class TrecPointerKey;
protected:
	UINT carotLoc;
	bool carotActive;

	bool grabbedRange;
	bool editAllowed;
public:
    explicit TInputTextElement(TrecPointer<DrawingBoard> board);
	bool FindString(const TString& target, UINT& index, bool fromFront = true);
	void UpdateCarotPoisition(UINT loc);

	bool GetCarotLoc(UINT& loc);
    virtual void OnDraw(TrecPointer<TVariable> dataText);

    virtual bool OnCLickDown(const TPoint& point);
    virtual bool OnCLickUp(const TPoint& point);
    virtual bool OnMouseMove(const TPoint& point);
    virtual void OnCutCopyPaste(control_text_mode mode);
    virtual bool OnInputChar(WCHAR ch, UINT count);

    virtual bool LockText(bool doLock);
    virtual bool TakesInput();
    virtual bool CanTakeInput();

    virtual void OnLoseFocus();

    virtual void OnTransferText(UINT newPos);
};

