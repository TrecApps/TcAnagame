#pragma once
#include "TPage.h"
class _ANA_WIDGET HandlerPage :
    public TPage
{
protected:
    TrecPointer<EventHandler> handler;

    TrecPointer<TPage> rootPage;

	void ClearEvents(TDataArray<EventID_Cred>& c);

public:

    HandlerPage(TrecPointer<DrawingBoard> board, TrecPointer<EventHandler> handler);

    bool HandlesEvents() override;
    void Draw(TrecPointer<TVariable> object) override;

	ag_msg void OnRButtonUp(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>&) override;
	ag_msg void OnRButtonDown(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>&) override;
	ag_msg void OnLButtonUp(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>&) override;
	ag_msg void OnLButtonDown(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>&) override;
	ag_msg void OnMouseMove(UINT nFlags, TPoint point, message_output& mOut, TDataArray<EventID_Cred>&) override;
	ag_msg void OnLButtonDblClk(UINT nFlags, TPoint point, message_output& mOut, TDataArray<EventID_Cred>& eventAr) override;
	ag_msg void OnResize(RECT_F& newLoc, UINT nFlags, TDataArray<EventID_Cred>& eventAr) override;
	ag_msg bool OnDestroy() override;
	ag_msg bool OnScroll(bool fromBars, const TPoint& point, const TPoint& direction, TDataArray<EventID_Cred>&) override;
};

