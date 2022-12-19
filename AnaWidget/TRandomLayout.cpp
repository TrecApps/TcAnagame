#include "TRandomLayout.h"

#define T_RANDOM_LAYOUT_LIMIT 10

bool TRandomLayout::onCreate(const RECT_F& loc, TrecPointer<TFileShell> d)
{
    bool res = TControl::onCreate(loc, d);

    for (UINT Rust = 0; Rust < childControls.Size(); Rust++)
    {
        auto page = childControls[Rust].control;
        if (dynamic_cast<TControl*>(page.Get()))
            res &= dynamic_cast<TControl*>(page.Get())->onCreate(loc, d);
    }
    return res;
}

TRandomLayout::TRandomLayout(TrecPointer<DrawingBoard> drawingBoard, TDataMap<TDataMap<TString>> styles) : TControl(drawingBoard, styles)
{
}

TRandomLayout::~TRandomLayout()
{
}

void TRandomLayout::Draw(TrecPointer<TVariable> object)
{
    TControl::Draw(object);
    auto var = TrecPointer<TVariable>();
    for (UINT Rust = 0; Rust < childControls.Size(); Rust++)
    {
        auto page = childControls[Rust].control;
        if (page.Get())
            page->Draw(object);
    }
}

void TRandomLayout::OnRButtonUp(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& cred)
{
    if (isRightClicked)
        TControl::OnRButtonUp(nFlags, point, mOut, cred);
    else
    {
        for (UINT Rust = 0; Rust < childControls.Size() && mOut == message_output::mo_negative; Rust++)
        {
            auto page = childControls[Rust].control;
            if (page.Get())
                page->OnRButtonUp(nFlags, point, mOut, cred);
        }

        if (mOut != message_output::mo_positive_override)
            TControl::OnRButtonUp(nFlags, point, mOut, cred);
    }
}

void TRandomLayout::OnRButtonDown(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& cred)
{
    for (UINT Rust = 0; Rust < childControls.Size() && mOut == message_output::mo_negative; Rust++)
    {
        auto page = childControls[Rust].control;
        if (page.Get())
            page->OnRButtonDown(nFlags, point, mOut, cred);
    }

    if (mOut != message_output::mo_positive_override)
        TControl::OnRButtonDown(nFlags, point, mOut, cred);
}

void TRandomLayout::OnLButtonUp(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& cred)
{

    for (UINT Rust = 0; Rust < childControls.Size() && mOut == message_output::mo_negative; Rust++)
    {
        auto page = childControls[Rust].control;
        if (page.Get())
            page->OnLButtonUp(nFlags, point, mOut, cred);
    }

    if (mOut != message_output::mo_positive_override)
        TControl::OnLButtonUp(nFlags, point, mOut, cred);

}

void TRandomLayout::OnMouseMove(UINT nFlags, TPoint point, message_output& mOut, TDataArray<EventID_Cred>& cred)
{
    if (!DrawingBoard::IsContained(point, area))
        return;

    for (UINT Rust = 0; Rust < childControls.Size() && mOut == message_output::mo_negative; Rust++)
    {
        auto page = childControls[Rust].control;
        if (page.Get())
            page->OnMouseMove(nFlags, point, mOut, cred);
    }

    if (mOut != message_output::mo_positive_override)
        TControl::OnMouseMove(nFlags, point, mOut, cred);
    
}

void TRandomLayout::OnLButtonDblClk(UINT nFlags, TPoint point, message_output& mOut, TDataArray<EventID_Cred>& args)
{
    for (UINT Rust = 0; Rust < childControls.Size() && mOut == message_output::mo_negative; Rust++)
    {
        auto page = childControls[Rust].control;
        if (page.Get())
            page->OnLButtonDblClk(nFlags, point, mOut, args);
    }

    if (mOut != message_output::mo_positive_override)
        TControl::OnLButtonDblClk(nFlags, point, mOut, args);
}

void TRandomLayout::OnLButtonDown(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& cred)
{
    for (UINT Rust = 0; Rust < childControls.Size() && mOut == message_output::mo_negative; Rust++)
    {
        auto page = childControls[Rust].control;
        if (page.Get())
            page->OnLButtonDown(nFlags, point, mOut, cred);
    }

    if (mOut != message_output::mo_positive_override)
        TControl::OnLButtonDown(nFlags, point, mOut, cred);
}

void TRandomLayout::OnResize(RECT_F& newLoc, UINT nFlags, TDataArray<EventID_Cred>& eventAr)
{
    for (UINT Rust = 0; Rust < childControls.Size(); Rust++)
    {
        auto page = childControls[Rust].control;
        if (page.Get())
            page->OnResize(newLoc, nFlags, eventAr);
    }

    TControl::OnResize(newLoc, nFlags, eventAr);
}

bool TRandomLayout::OnScroll(bool, const TPoint& point, const TPoint& direction, TDataArray<EventID_Cred>& args)
{
    if (TControl::OnScroll(false, point, direction, args))
        return false;

    for (UINT Rust = 0; Rust < childControls.Size(); Rust++)
    {
        auto page = childControls[Rust].control;
        if (page.Get())
            page->OnScroll(false, point, direction, args);
    }
    return true;
}

bool TRandomLayout::AddPage(TrecPointer<TPage> page, UINT row, UINT col, bool doOverride)
{
    if (!page.Get())
        return false;
    ChildControl cc;
    cc.col = col;
    cc.row = row;
    cc.control = page;

    for (UINT Rust = 0; Rust < childControls.Size(); Rust++)
    {
        auto page = childControls[Rust].control;
        if (!page.Get())
        {
            childControls[Rust] = cc;
            return true;
        }
    }
    if (childControls.Size() >= T_RANDOM_LAYOUT_LIMIT)
        return false;

    childControls.push_back(cc);
    if (page.Get())
        page->SetParentPage(TrecPointerKey::TrecFromSoft<>(self));
    return true;
}

bool TRandomLayout::RemovePage(TrecPointer<TPage> page)
{
    for (UINT Rust = 0; Rust < childControls.Size(); Rust++)
    {
        if (page.Get() == childControls[Rust].control.Get())
        {
            childControls[Rust].control.Nullify();
            return true;
        }
    }
    return false;
}

bool TRandomLayout::RemovePage(UINT row, UINT col)
{
    if (row < childControls.Size())
    {
        childControls[row].control.Nullify();
        return true;
    }
    return false;
}

void TRandomLayout::GetControlCapacity(UINT& row, UINT& col)
{
    row = T_RANDOM_LAYOUT_LIMIT;
    col = 1;
}

TrecPointer<TPage> TRandomLayout::GetPage(UINT row, UINT col)
{
    if (row < childControls.Size())
    {
        return childControls[row].control;
    }
    return TrecPointer<TPage>();
}

TrecPointer<TControl> TRandomLayout::GetControlById(const TString& id)
{
    TrecPointer<TControl> ret = TControl::GetControlById(id);

    for (UINT Rust = 0; !ret.Get() && Rust < childControls.Size(); Rust++)
    {
        TrecPointer<TControl> cc = TrecPointerKey::ConvertPointer<TPage, TControl>(childControls[Rust].control);
        if (cc.Get())
        {
            ret = cc->GetControlById(id);
        }
    }
    return ret;
}

ChildControl::ChildControl()
{
    row = col = 0;
}

ChildControl::ChildControl(const ChildControl& copy)
{
    row = copy.row;
    col = copy.col;
    control = copy.control;
}
