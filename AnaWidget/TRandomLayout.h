#pragma once
#include "TControl.h"

/**
 * Class: ChildControl
 * Purpose: Holds a Control
 */
class _ANA_WIDGET ChildControl
{
public:
	ChildControl();
	ChildControl(const ChildControl& copy);
	TrecPointer<TPage> control;
	UINT row, col;
};

/**
 * Class: TRandomLayout
 * Purpose: Extends Anaface 2's TControls to support Child Controls
 */
class _ANA_WIDGET TRandomLayout :
	public TControl
{
public:

	virtual bool onCreate(const RECT_F& loc, TrecPointer<TFileShell> d)override;

	TRandomLayout(TrecPointer<DrawingBoard> drawingBoard, TDataMap<TDataMap<TString>> styles);

	virtual ~TRandomLayout();

	virtual void Draw(TrecPointer<TVariable> object) override;

	ag_msg virtual void OnRButtonUp(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>&) override;

	ag_msg virtual void OnRButtonDown(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>&) override;

	ag_msg virtual void OnLButtonUp(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>&) override;

	ag_msg virtual void OnMouseMove(UINT nFlags, TPoint point, message_output& mOut, TDataArray<EventID_Cred>&) override;

	ag_msg virtual void OnLButtonDblClk(UINT nFlags, TPoint point, message_output& mOut, TDataArray<EventID_Cred>& args) override;

	ag_msg virtual void OnLButtonDown(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& args) override;

	ag_msg virtual void OnResize(RECT_F& newLoc, UINT nFlags, TDataArray<EventID_Cred>& eventAr) override;

	ag_msg virtual bool OnScroll(bool, const TPoint& point, const TPoint& direction, TDataArray<EventID_Cred>& args) override;

	virtual bool AddPage(TrecPointer<TPage> page, UINT row, UINT col, bool doOverride = false);

	bool RemovePage(TrecPointer<TPage> page);

	virtual bool RemovePage(UINT row, UINT col);

	virtual void GetControlCapacity(UINT& row, UINT& col);

	virtual TrecPointer<TPage> GetPage(UINT row, UINT col);

	virtual TrecPointer<TControl> GetControlById(const TString& id);

protected:

	/**
	 * Child Controls
	 */
	TDataArray<ChildControl> childControls;
};

