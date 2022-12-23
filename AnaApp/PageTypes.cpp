#include "PageTypes.h"

TPageEx::TPageEx(TrecActivePointer<TPage> page, const TString& name) : TPage(page.GetTrecPointer()->GetDrawingBoard())
{
    this->mainPage = page.GetTrecPointer();
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
    mainPage->OnRButtonUp(nFlags, point, mOut, cred);
}

ag_msg void TPageEx::OnRButtonDown(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& cred)
{
    mainPage->OnRButtonDown(nFlags, point, mOut, cred);
}

ag_msg void TPageEx::OnLButtonUp(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& cred)
{
    mainPage->OnLButtonUp(nFlags, point, mOut, cred);
}
ag_msg void TPageEx::OnLButtonDown(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& cred)
{
    mainPage->OnLButtonDown(nFlags, point, mOut, cred);
}

ag_msg void TPageEx::OnMouseMove(UINT nFlags, TPoint point, message_output& mOut, TDataArray<EventID_Cred>& cred)
{
    mainPage->OnMouseMove(nFlags, point, mOut, cred);
}

ag_msg void TPageEx::OnLButtonDblClk(UINT nFlags, TPoint point, message_output& mOut, TDataArray<EventID_Cred>& eventAr)
{
    mainPage->OnLButtonDblClk(nFlags, point, mOut, eventAr);
}

ag_msg void TPageEx::OnResize(RECT_F& newLoc, UINT nFlags, TDataArray<EventID_Cred>& eventAr)
{
    mainPage->OnResize(newLoc, nFlags, eventAr);
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

TRibbonPage::TRibbonPage(TrecActivePointer<TPage> page, const TString& name, bool isMain) : TPageEx(page, name)
{
    this->isMain = isMain;
}

page_type TRibbonPage::GetPageType()
{
    return isMain ? page_type::main_ribbon : page_type::ribbon_sup;
}
