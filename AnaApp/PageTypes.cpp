#include "PageTypes.h"

TPageEx::TPageEx(TrecActivePointer<TPage> page, const TString& name) : TPage(TrecPointerKey::ActiveToTrec<>(page)->GetDrawingBoard())
{
    this->mainPage = TrecPointerKey::ActiveToTrec<>(page);
    this->name.Set(name);
    this->area = mainPage->GetArea();
}

TrecPointer<TPage> TPageEx::GetPage()
{
    return mainPage;
}

TString TPageEx::GetName()
{
    return name;
}

bool TPageEx::HandlesEvents()
{
    return mainPage->HandlesEvents();
}

void TPageEx::Draw(TrecPointer<TVariable> object)
{
    mainPage->Draw(object);
}

ag_msg void TPageEx::OnRButtonUp(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& cred)
{
    OnRButtonUp(nFlags, point, mOut, cred);
}

ag_msg void TPageEx::OnRButtonDown(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& cred)
{
    OnRButtonDown(nFlags, point, mOut, cred);
}

ag_msg void TPageEx::OnLButtonUp(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& cred)
{
    OnLButtonUp(nFlags, point, mOut, cred);
}
ag_msg void TPageEx::OnLButtonDown(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& cred)
{
    OnLButtonDown(nFlags, point, mOut, cred);
}

ag_msg void TPageEx::OnMouseMove(UINT nFlags, TPoint point, message_output& mOut, TDataArray<EventID_Cred>& cred)
{
    OnMouseMove(nFlags, point, mOut, cred);
}

ag_msg void TPageEx::OnLButtonDblClk(UINT nFlags, TPoint point, message_output& mOut, TDataArray<EventID_Cred>& eventAr)
{
    OnLButtonDblClk(nFlags, point, mOut, eventAr);
}

ag_msg void TPageEx::OnResize(RECT_F& newLoc, UINT nFlags, TDataArray<EventID_Cred>& eventAr)
{
    OnResize(newLoc, nFlags, eventAr);
    area = mainPage->GetArea();
}

ag_msg bool TPageEx::OnDestroy()
{
    return mainPage->OnDestroy();
}

ag_msg bool TPageEx::OnScroll(bool fromBars, const TPoint& point, const TPoint& direction, TDataArray<EventID_Cred>& cred)
{
    bool ret = mainPage->OnScroll(fromBars, point, direction, cred);
    area = mainPage->GetArea();
    return ret;
}
