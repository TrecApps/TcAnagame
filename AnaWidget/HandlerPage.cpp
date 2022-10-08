#include "HandlerPage.h"
#include <cassert>

void HandlerPage::ClearEvents(TDataArray<EventID_Cred>& c)
{
	for (UINT Rust = 0; Rust < c.Size(); Rust++)
	{
		c[Rust].args.Nullify();
	}
}

HandlerPage::HandlerPage(TrecPointer<DrawingBoard> board, TrecPointer<EventHandler> handler): TPage(board)
{
	this->handler = handler;
	assert(handler.Get());
}

bool HandlerPage::HandlesEvents()
{
	return true;
}

void HandlerPage::Draw(TrecPointer<TVariable> object)
{
	if (rootPage.Get())
		rootPage->Draw(object);
}

ag_msg void HandlerPage::OnRButtonUp(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& c)
{
	if (rootPage.Get())
	{
		rootPage->OnRButtonUp(nFlags, point, mOut, c);

		handler->HandleEvents(c);
		ClearEvents(c);
	}
}

ag_msg void HandlerPage::OnRButtonDown(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& c)
{
	if (rootPage.Get())
	{
		rootPage->OnRButtonDown(nFlags, point, mOut, c);

		handler->HandleEvents(c);
		ClearEvents(c);
	}
}

ag_msg void HandlerPage::OnLButtonUp(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& c)
{
	if (rootPage.Get())
	{
		rootPage->OnLButtonUp(nFlags, point, mOut, c);

		handler->HandleEvents(c);
		ClearEvents(c);
	}
}

ag_msg void HandlerPage::OnLButtonDown(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& c)
{
	if (rootPage.Get())
	{
		rootPage->OnLButtonDown(nFlags, point, mOut, c);

		handler->HandleEvents(c);
		ClearEvents(c);
	}
}

ag_msg void HandlerPage::OnMouseMove(UINT nFlags, TPoint point, message_output& mOut, TDataArray<EventID_Cred>& c)
{
	if (rootPage.Get())
	{
		rootPage->OnMouseMove(nFlags, point, mOut, c);

		handler->HandleEvents(c);
		ClearEvents(c);
	}

}

ag_msg void HandlerPage::OnLButtonDblClk(UINT nFlags, TPoint point, message_output& mOut, TDataArray<EventID_Cred>& c)
{
	if (rootPage.Get())
	{
		rootPage->OnLButtonDblClk(nFlags, point, mOut, c);

		handler->HandleEvents(c);
		ClearEvents(c);
	}
}

ag_msg void HandlerPage::OnResize(RECT_F& newLoc, UINT nFlags, TDataArray<EventID_Cred>& c)
{
	this->area = newLoc;
	if (rootPage.Get())
	{
		rootPage->OnResize(newLoc, nFlags, c);

		handler->HandleEvents(c);
		ClearEvents(c);
	}
}

ag_msg bool HandlerPage::OnDestroy()
{
	return rootPage.Get() ? rootPage->OnDestroy() : true;
}

ag_msg bool HandlerPage::OnScroll(bool fromBars, const TPoint& point, const TPoint& direction, TDataArray<EventID_Cred>& c)
{
	area.bottom += direction.y;
	area.top += direction.y;
	area.left += direction.x;
	area.right += direction.x;

	if (rootPage.Get())
		rootPage->OnScroll(fromBars, point, direction, c);
	return true;
}
