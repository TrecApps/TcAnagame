#include "TScrollerPage.h"

void TScrollerPage::RefreshScroll()
{
	ThreadLock();
	if (!childPage.Get())
	{
		ThreadRelease();
		return;
	}
	vScroll.Nullify();
	hScroll.Nullify();
	RECT_F childLocation = childPage->GetArea();
	if (!vScroll.Get() && ((area.bottom - area.top) < (childLocation.bottom - childLocation.top)))
	{
		// We need to create the Vertical ScrollBar
		vScroll = TrecPointerKey::GetNewTrecPointer<TScrollBar>(TrecPointerKey::TrecFromSoft<>(self), ScrollOrient::so_vertical);
	}

	if (!hScroll.Get() && ((area.right - area.left) < (childLocation.right - childLocation.left)))
	{
		// We need to create the horizontal ScrollBar
		hScroll = TrecPointerKey::GetNewTrecPointer<TScrollBar>(TrecPointerKey::TrecFromSoft<>(self), ScrollOrient::so_horizontal);
	}

	//if (vScroll && ((location.bottom - location.top) > (childLocation.bottom - childLocation.top)))
	//{

	//}


	RECT_F clipRect = area;
	if (vScroll.Get())
		clipRect.bottom -= GetScrollbarBoxSize();
	if (hScroll.Get())
		clipRect.right -= GetScrollbarBoxSize();

	if (vScroll.Get())
		vScroll->Refresh(clipRect, childLocation);
	if (hScroll.Get())
		hScroll->Refresh(clipRect, childLocation);
	ThreadRelease();
}

TrecPointer<TPage> TScrollerPage::GetChildPage()
{
	return childPage;
}

TScrollerPage::TScrollerPage(TrecPointer<DrawingBoard> board, TrecPointer<TPage> cPage) :TPage(board)
{
	if (!cPage.Get())
		throw L"Null Pointer";
	childPage = cPage;
}

bool TScrollerPage::IsScroller()
{
	return true;
}

void TScrollerPage::Draw(TrecPointer<TVariable> object)
{
	TObjectLocker lock(&this->thread);
	if ((area.bottom - area.top) < 1 || (area.right - area.left) < 1)
	{
		return;
	}


	RefreshScroll();
	if (vScroll.Get() || hScroll.Get())
	{

		RECT_F clipRect = area;
		clipRect.bottom -= GetScrollbarBoxSize();
		clipRect.right -= GetScrollbarBoxSize();

		// Need to generate a geometry
		drawingBoard->AddLayer(clipRect);

		if (vScroll.Get())
			vScroll->Refresh(clipRect, childPage->GetArea());
		if (hScroll.Get())
			hScroll->Refresh(clipRect, childPage->GetArea());
	}
	childPage->Draw(object);
	if (vScroll.Get() || hScroll.Get())
	{
		drawingBoard->PopLayer();
	}
	if (vScroll.Get())
		vScroll->onDraw(drawingBoard);
	if (hScroll.Get())
		hScroll->onDraw(drawingBoard);

}

bool TScrollerPage::HandlesEvents()
{
	return false;
}

ag_msg void TScrollerPage::OnRButtonUp(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& cred)
{
	TObjectLocker lock(&this->thread);
	childPage->OnRButtonUp(nFlags, point, mOut, cred);
}

ag_msg void TScrollerPage::OnRButtonDown(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& cred)
{
	TObjectLocker lock(&this->thread);
	childPage->OnRButtonDown(nFlags, point, mOut, cred);
}

ag_msg void TScrollerPage::OnLButtonUp(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& cred)
{
	TObjectLocker lock(&this->thread);
	childPage->OnLButtonUp(nFlags, point, mOut, cred);
}

ag_msg void TScrollerPage::OnLButtonDown(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& cred)
{
	TObjectLocker lock(&this->thread);
	if (vScroll.Get() && vScroll->OnLButtonDown(nFlags, point, mOut))
	{
		EventArgs args_;
		args_.Reset();
		args_.eventType = R_Message_Type::On_Select_Scroller;
		args_.point = point;
		args_.methodID = -1;
		args_.isClick = args_.isLeftClick = true;

		EventID_Cred _event(EventID_Cred(R_Message_Type::On_Select_Scroller, TrecPointerKey::TrecFromSoft<>(self), vScroll));
		_event.args = TrecPointerKey::GetNewTrecPointer<EventArgs>(args_);
		cred.push_back(_event);
		return;
	}


	if (hScroll.Get() && hScroll->OnLButtonDown(nFlags, point, mOut))
	{
		EventArgs args_;
		args_.Reset();
		args_.eventType = R_Message_Type::On_Select_Scroller;
		args_.point = point;
		args_.methodID = -1;
		args_.isClick = args_.isLeftClick = true;

		EventID_Cred _event(EventID_Cred(R_Message_Type::On_Select_Scroller, TrecPointerKey::TrecFromSoft<>(self), hScroll));
		_event.args = TrecPointerKey::GetNewTrecPointer<EventArgs>(args_);
		cred.push_back(_event);
		return;
	}

	if (childPage.Get())
		childPage->OnLButtonDown(nFlags, point, mOut, cred);
}

ag_msg void TScrollerPage::OnMouseMove(UINT nFlags, TPoint point, message_output& mOut, TDataArray<EventID_Cred>& cred)
{
	TObjectLocker lock(&this->thread);
	childPage->OnMouseMove(nFlags, point, mOut, cred);
}

ag_msg void TScrollerPage::OnLButtonDblClk(UINT nFlags, TPoint point, message_output& mOut, TDataArray<EventID_Cred>& eventAr)
{
	TObjectLocker lock(&this->thread);
	childPage->OnLButtonDblClk(nFlags, point, mOut, eventAr);
}

ag_msg void TScrollerPage::OnResize(RECT_F& newLoc, UINT nFlags, TDataArray<EventID_Cred>& eventAr)
{
	TObjectLocker lock(&this->thread);
	area = newLoc;
	childPage->OnResize(newLoc, nFlags, eventAr);

	RefreshScroll();
}

ag_msg bool TScrollerPage::OnDestroy()
{
	TObjectLocker lock(&this->thread);
	return childPage->OnDestroy();
}

ag_msg bool TScrollerPage::OnScroll(bool fromBars, const TPoint& point, const TPoint& direction, TDataArray<EventID_Cred>& eventAr)
{
	TObjectLocker lock(&this->thread);

	if (!fromBars)
	{
		area.bottom += direction.y;
		area.top += direction.y;
		area.left += direction.x;
		area.right += direction.x;
	}
	return childPage->OnScroll(false, point, direction, eventAr);


}
