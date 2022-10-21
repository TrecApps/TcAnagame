#pragma once
#include "TControl.h"


class _ANA_WIDGET TGadget :
	public TControl
{
public:

	TGadget(TrecPointer<DrawingBoard> rt, TDataMap<TDataMap<TString>> ta);

	virtual ~TGadget();

	bool onCreate(const RECT_F& loc, TrecPointer<TFileShell> d)override;

	ag_msg virtual void OnResize(RECT_F& newLoc, UINT nFlags, TDataArray<EventID_Cred>& eventAr) override;

protected:

	/**
	 * Intended size of the gadget
	 */
	int bSize;

	/**
	 * Brish to drw the inner gadget with
	 */
	TrecPointer<TColorBrush> brush;

	/**
	 * Thickness of the inner box lines
	 */
	float thickness;

	/**
	 * Color of the inner box
	 */
	TColor dxColor;
	/**
	 * Location of the inner box
	 */
	RECT_F DxLocation;
};

