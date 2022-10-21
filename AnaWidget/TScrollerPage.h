#pragma once
#include "TPage.h"
class _ANA_WIDGET TScrollerPage :
	public TPage
{
protected:
	/**
	 * The Child Page to scroll
	 */
	TrecPointer<TPage> childPage;

	/**
	 * The Scroll Bars
	 */
	TrecPointer<TPage::TScrollBar> hScroll, vScroll;

	void RefreshScroll();
public:

	TrecPointer<TPage> GetChildPage();

	TScrollerPage(TrecPointer<DrawingBoard> board, TrecPointer<TPage> cPage);

	virtual bool IsScroller() override;

	virtual void Draw(TrecPointer<TVariable> object) override;

	virtual bool HandlesEvents() override;

	ag_msg virtual void OnRButtonUp(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>&) override;

	ag_msg virtual void OnRButtonDown(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>&) override;

	ag_msg virtual void OnLButtonUp(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>&) override;

	ag_msg virtual void OnLButtonDown(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>&) override;

	ag_msg virtual void OnMouseMove(UINT nFlags, TPoint point, message_output& mOut, TDataArray<EventID_Cred>&) override;

	ag_msg virtual void OnLButtonDblClk(UINT nFlags, TPoint point, message_output& mOut, TDataArray<EventID_Cred>& eventAr) override;

	ag_msg virtual void OnResize(RECT_F& newLoc, UINT nFlags, TDataArray<EventID_Cred>& eventAr) override;

	ag_msg virtual bool OnDestroy() override;

	ag_msg virtual bool OnScroll(bool fromBars, const TPoint& point, const TPoint& direction, TDataArray<EventID_Cred>&) override;
};

