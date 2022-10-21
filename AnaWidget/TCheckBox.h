#pragma once
#include "TGadget.h"
class _ANA_WIDGET TCheckBox :
	public TGadget
{
public:

	TCheckBox(TrecPointer<DrawingBoard> rt, TDataMap<TDataMap<TString>> ta);

	virtual ~TCheckBox();

	virtual bool onCreate(const RECT_F& loc, TrecPointer<TFileShell> d)override;

	virtual void Draw(TrecPointer<TVariable> object) override;

	bool IsChecked();

	ag_msg virtual void OnLButtonUp(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>&) override;

	ag_msg virtual void OnResize(RECT_F& newLoc, UINT nFlags, TDataArray<EventID_Cred>& eventAr) override;

protected:

	bool isChecked;
};

