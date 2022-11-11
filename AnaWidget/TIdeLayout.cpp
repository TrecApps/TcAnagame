#include "TIdeLayout.h"

bool DivideRectangle(const RECT_F& rect, bool vertical, bool isTotal, float& divide)
{
    float outer = vertical ? rect.right : rect.bottom;
    float inner = vertical ? rect.left : rect.top;

    if (isTotal)
    {
        divide /= (outer - inner);
    }
    if (divide < 1.0f)
        divide *= 100.f;

    if (divide < 5.0f || divide > 95.f)
        return false;

    divide /= 100.f;

    divide = (inner + (outer - inner) * divide);
    return true;
}

bool TIdeLayout::onCreate(const RECT_F& loc, TrecPointer<TFileShell> d)
{
    TRandomLayout::onCreate(loc, d);

    TString valpoint;
    if (attributes.retrieveEntry(L"BorderThickness", valpoint))
    {
        valpoint.ConvertToFloat(thickness);
    }
    TColor col;
    if (attributes.retrieveEntry(L"BorderColor", valpoint))
    {
        bool w;
        col = TColor::GetColorFromString(valpoint, w);
    }
    dividerColor = TrecPointerKey::ConvertPointer<TBrush, TColorBrush>(drawingBoard->GetSolidColorBrush(col));

    if (attributes.retrieveEntry(L"MouseSensitivity", valpoint))
    {
        valpoint.ConvertToFloat(mouseSensitivity);
    }
    return false;
}

TIdeLayout::TIdeLayout(TrecPointer<DrawingBoard> drawingBoard, TDataMap<TDataMap<TString>> styles): TRandomLayout(drawingBoard, styles)
{
    thickness = 2.0f;
    mouseSensitivity = thickness;
    dividerMovable = true;
    structureFixed = true;
}

TIdeLayout::~TIdeLayout()
{
}

void TIdeLayout::Draw(TrecPointer<TVariable> object)
{
    TRandomLayout::Draw(object);
    if (rootSection.Get())
        rootSection->Draw(object, this->dividerColor, this->thickness);
}

void TIdeLayout::OnRButtonUp(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& er)
{
    TRandomLayout::OnRButtonUp(nFlags, point, mOut, er);
    if (!DrawingBoard::IsContained(point, area))
        return;

    if (rootSection.Get())
        rootSection->OnClickEvent(click_section_event::right_up, nFlags, point, mOut, er);
}

void TIdeLayout::OnRButtonDown(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& er)
{
    TRandomLayout::OnRButtonDown(nFlags, point, mOut, er);
    if (!DrawingBoard::IsContained(point, area))
        return;

    if (rootSection.Get())
        rootSection->OnClickEvent(click_section_event::right_down, nFlags, point, mOut, er);
}

void TIdeLayout::OnLButtonUp(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& er)
{
    this->captured.Nullify();
    TRandomLayout::OnLButtonUp(nFlags, point, mOut, er);
    if (!DrawingBoard::IsContained(point, area))
        return;

    if (rootSection.Get())
        rootSection->OnClickEvent(click_section_event::left_up, nFlags, point, mOut, er);
}

void TIdeLayout::OnMouseMove(UINT nFlags, TPoint point, message_output& mOut, TDataArray<EventID_Cred>& er)
{
    if (!DrawingBoard::IsContained(point, area))
        return;

    if (captured.Get())
    {
        float newBarrier = captured->isVertical ? (point.x - captured->bounds.left) : (point.y - captured->bounds.top);

        if (DivideRectangle(captured->bounds, captured->isVertical, true, newBarrier))
        {
            captured->leftTop = newBarrier;
            if (captured->first.Get())
            {
                RECT_F r = captured->bounds;
                if (captured->isVertical)
                    r.right = newBarrier;
                else
                    r.bottom = newBarrier;
                captured->first->OnResize(r, 0, er);
            }if (captured->second.Get())
            {
                RECT_F r = captured->bounds;
                if (captured->isVertical)
                    r.left = newBarrier;
                else
                    r.top = newBarrier;
                captured->second->OnResize(r, 0, er);
            }
        }
    }
    else if (rootSection.Get())
        rootSection->OnMouseMove(nFlags, point, mOut, er);
}

void TIdeLayout::OnLButtonDblClk(UINT nFlags, TPoint point, message_output& mOut, TDataArray<EventID_Cred>& args)
{
    TRandomLayout::OnLButtonDblClk(nFlags, point, mOut, args);
    if (!DrawingBoard::IsContained(point, area))
        return;
    if (rootSection.Get())
        rootSection->OnLButtonDblClk(nFlags, point, mOut, args);
}

void TIdeLayout::OnLButtonDown(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& args)
{
    TRandomLayout::OnLButtonDown(nFlags, point, mOut, args);
    if (!DrawingBoard::IsContained(point, area))
        return;
    if (rootSection.Get())
        rootSection->OnClickEvent(click_section_event::left_down, nFlags, point, mOut, args);
    RECT_F r;
    TrecPointer<IdeSection> section;
    if (GetBounds(point, r, section) && dynamic_cast<IdeDividerSection*>(section.Get()))
        captured = TrecPointerKey::ConvertPointer<IdeSection, IdeDividerSection>(section);
}

void TIdeLayout::OnResize(RECT_F& newLoc, UINT nFlags, TDataArray<EventID_Cred>& eventAr)
{
    TRandomLayout::OnResize(newLoc, nFlags, eventAr);
    if (rootSection.Get())
        rootSection->OnResize(newLoc, nFlags, eventAr);
}

bool TIdeLayout::OnScroll(bool, const TPoint& point, const TPoint& direction, TDataArray<EventID_Cred>& args)
{
    return false;
}

bool TIdeLayout::AddPage(TrecPointer<TPage> page, UINT row, UINT col, bool doOverride)
{
    return false;
}



bool TIdeLayout::RemovePage(UINT row, UINT col)
{
    return false;
}

void TIdeLayout::GetControlCapacity(UINT& row, UINT& col)
{
    row = col = 0;
}

TrecPointer<TPage> TIdeLayout::GetPage(UINT row, UINT col)
{
    return TrecPointer<TPage>();
}

TrecPointer<IdeSection> TIdeLayout::GetRootSection()
{
    return rootSection;
}

bool TIdeLayout::AppendSection(TrecPointer<IdeSection> section, TrecPointer<AnafacePage> page)
{
    if (!page.Get())return false;
    if (!section.Get() || section->GetSectionType() != ide_section_type::divider)
        return false;

    TrecPointer<IdeDividerSection> divSection = TrecPointerKey::ConvertPointer<IdeSection, IdeDividerSection>(section);

    if (divSection->first.Get() && divSection->second.Get())
        return false;
    TrecPointer<IdeTabSection> pageSection = TrecPointerKey::GetNewTrecPointer<IdeTabSection>();
    pageSection->tab = page;
    TrecPointer<IdeSection> section = TrecPointerKey::ConvertPointer<IdeTabSection, IdeSection>(pageSection);
    section->bounds = section->bounds;
    if (!divSection->first.Get())
    {
        if (divSection->isVertical)
            section->bounds.right = divSection->leftTop;
        else
            section->bounds.bottom = divSection->leftTop;
        divSection->first = section;
    }
    else
    {
        if (divSection->isVertical)
            section->bounds.left = divSection->leftTop;
        else
            section->bounds.top = divSection->leftTop;
        divSection->second = section;
    }
    TDataArray<TPage::EventID_Cred> cred;
    page->OnResize(section->bounds, 0, cred);
}

bool TIdeLayout::AppendSection(TrecPointer<IdeSection> section, TrecPointer<TPage> page)
{
    if (!page.Get())return false;
    if (!section.Get() || section->GetSectionType() != ide_section_type::divider)
        return false;

    TrecPointer<IdeDividerSection> divSection = TrecPointerKey::ConvertPointer<IdeSection, IdeDividerSection>(section);

    if (divSection->first.Get() && divSection->second.Get())
        return false;
    TrecPointer<IdePageSection> pageSection = TrecPointerKey::GetNewTrecPointer<IdePageSection>();
    pageSection->tab = page;
    TrecPointer<IdeSection> section = TrecPointerKey::ConvertPointer<IdePageSection, IdeSection>(pageSection);
    section->bounds = section->bounds;
    if (!divSection->first.Get())
    {
        if (divSection->isVertical)
            section->bounds.right = divSection->leftTop;
        else
            section->bounds.bottom = divSection->leftTop;
        divSection->first = section;
    }
    else
    {
        if (divSection->isVertical)
            section->bounds.left = divSection->leftTop;
        else
            section->bounds.top = divSection->leftTop;
        divSection->second = section;
    }
    TDataArray<TPage::EventID_Cred> cred;
    page->OnResize(section->bounds, 0, cred);
}

bool TIdeLayout::DivideSection(TrecPointer<IdeSection> section, bool verticle, bool totalSpace, float dividePoint)
{
    if(!section.Get() || section->GetSectionType() != ide_section_type::divider)
        return false;

    TrecPointer<IdeDividerSection> divSection = TrecPointerKey::ConvertPointer<IdeSection, IdeDividerSection>(section);

    RECT_F curBounds = section->bounds;

    TrecPointer<IdeSection> newSection = divSection->first;


    bool doFirst = true;
    if (!newSection.Get())
    {
        if (divSection->isVertical)
            curBounds.right = divSection->leftTop;
        else
            curBounds.bottom = divSection->leftTop;
    }
    else
    {
        doFirst = false;
        newSection = divSection->second;
        if (divSection->isVertical)
            curBounds.left = divSection->leftTop;
        else
            curBounds.top = divSection->leftTop;
    }

    if (newSection.Get() || !DivideRectangle(curBounds, verticle, totalSpace, dividePoint))
        return false;

    TrecPointer<IdeDividerSection> divSectionNew = TrecPointerKey::GetNewTrecPointer<IdeDividerSection>();
    divSectionNew->isVertical = verticle;
    divSectionNew->leftTop = dividePoint;
    newSection = TrecPointerKey::ConvertPointer<IdeDividerSection, IdeSection>(divSectionNew);
    newSection->bounds = curBounds;

    if (doFirst)
        divSection->first = newSection;
    else divSection->second = newSection;
}

bool TIdeLayout::GetBounds(const TPoint& point, RECT_F& bounds, TrecPointer<IdeSection>& section)
{
    section.Nullify();

    if (!DrawingBoard::IsContained(point, area))
        return false;

    bounds = area;

    if (!rootSection.Get())
    {
        return true;
    }

    section = rootSection;

    while (section.Get())
    {
        bounds = section->bounds;

        TrecPointer<IdeDividerSection> divSection;

        switch (section->GetSectionType())
        {
        case ide_section_type::tab:
        case ide_section_type::page:
            return true;
        case ide_section_type::divider:
            divSection = TrecPointerKey::ConvertPointer<IdeSection, IdeDividerSection>(section);
            if (divSection->isVertical)
            {
                if (abs(divSection->leftTop - point.x) < this->mouseSensitivity)
                    return true;

                if (point.x < divSection->leftTop)
                    section = divSection->first;
                else
                    section = divSection->second;
            }
            else
            {
                if (abs(divSection->leftTop - point.y) < this->mouseSensitivity)
                    return true;

                if (point.y < divSection->leftTop)
                    section = divSection->first;
                else
                    section = divSection->second;
            }
        }
    }

    return true;
}

bool TIdeLayout::GetBounds(TrecPointer<IdeSection> section, RECT_F& bounds)
{
    if(!section.Get())
    return false;
    bounds = section->bounds;
    return true;
}

ide_section_type IdeDividerSection::GetSectionType()
{
    return ide_section_type::divider;
}

void IdeDividerSection::OnClickEvent(click_section_event clickEvent, UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<TPage::EventID_Cred>& er)
{
    if (first.Get())
        first->OnClickEvent(clickEvent, nFlags, point, mOut, er);
    if(second.Get())
        second->OnClickEvent(clickEvent, nFlags, point, mOut, er);
}

void IdeDividerSection::OnMouseMove(UINT nFlags, TPoint point, message_output& mOut, TDataArray<TPage::EventID_Cred>& er)
{
    if (first.Get())
        first->OnMouseMove(nFlags, point, mOut, er);
    if (second.Get())
        second->OnMouseMove(nFlags, point, mOut, er);
}

void IdeDividerSection::OnLButtonDblClk(UINT nFlags, TPoint point, message_output& mOut, TDataArray<TPage::EventID_Cred>& er)
{
    if (first.Get())
        first->OnLButtonDblClk(nFlags, point, mOut, er);
    if (second.Get())
        second->OnLButtonDblClk(nFlags, point, mOut, er);
}

void IdeDividerSection::OnResize(RECT_F& newLoc, UINT nFlags, TDataArray<TPage::EventID_Cred>& eventAr)
{
    float widthFactor = (newLoc.right - newLoc.left) / (bounds.right - bounds.left);
    float heightFactor = (newLoc.bottom - newLoc.top) / (bounds.bottom - bounds.top);

    this->leftTop = (leftTop - (isVertical ? bounds.left : bounds.top)) * (isVertical ? widthFactor : heightFactor);

    this->bounds = newLoc;

    if (first.Get())
    {
        RECT_F chBounds = bounds;
        if (isVertical)
            chBounds.right = leftTop;
        else
            chBounds.bottom = leftTop;
        first->OnResize(chBounds, nFlags, eventAr);
    }
    if (second.Get())
    {
        RECT_F chBounds = bounds;
        if (isVertical)
            chBounds.left = leftTop;
        else
            chBounds.top = leftTop;
        second->OnResize(chBounds, nFlags, eventAr);
    }

}

void IdeDividerSection::Draw(TrecPointer<TVariable> obj, TrecPointer<TColorBrush> col, float thickness)
{
    TPoint p1, p2;

    if (!col.Get())
        return;

    if (isVertical)
    {
        p1.x = p2.x = leftTop;
        p1.y = bounds.top;
        p2.y = bounds.bottom;
    }
    else
    {
        p1.y = p2.y = leftTop;
        p1.x = bounds.left;
        p2.x = bounds.right;
    }

    col->DrawLine(p1, p2, thickness);

    if (first.Get())
        first->Draw(obj, col, thickness);
    if (second.Get())
        second->Draw(obj, col, thickness);
}

ide_section_type IdeTabSection::GetSectionType()
{
    return ide_section_type::tab;
}

void IdeTabSection::OnClickEvent(click_section_event clickEvent, UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<TPage::EventID_Cred>& er)
{
    if (tab.Get())
    {
        switch (clickEvent)
        {
        case click_section_event::left_down:
            tab->OnLButtonDown(nFlags, point, mOut, er);
            break;
        case click_section_event::left_up:
            tab->OnLButtonUp(nFlags, point, mOut, er);
            break;
        case click_section_event::right_down:
            tab->OnRButtonDown(nFlags, point, mOut, er);
            break;
        case click_section_event::right_up:
            tab->OnRButtonUp(nFlags, point, mOut, er);
        }
    }
}

void IdeTabSection::OnMouseMove(UINT nFlags, TPoint point, message_output& mOut, TDataArray<TPage::EventID_Cred>& er)
{
    if (tab.Get())
        tab->OnMouseMove(nFlags, point, mOut, er);
}

void IdeTabSection::OnLButtonDblClk(UINT nFlags, TPoint point, message_output& mOut, TDataArray<TPage::EventID_Cred>& er)
{
    if (tab.Get())
        tab->OnLButtonDblClk(nFlags, point, mOut, er);
}

void IdeTabSection::OnResize(RECT_F& newLoc, UINT nFlags, TDataArray<TPage::EventID_Cred>& eventAr)
{
    if (this->tab.Get())
        tab->OnResize(newLoc, nFlags, eventAr);
    bounds = newLoc;
}

void IdeTabSection::Draw(TrecPointer<TVariable> obj, TrecPointer<TColorBrush> col, float thickness)
{
    if (tab.Get())
        tab->Draw(obj);
}

ide_section_type IdePageSection::GetSectionType()
{
    return ide_section_type::page;
}

void IdePageSection::OnClickEvent(click_section_event clickEvent, UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<TPage::EventID_Cred>& er)
{
    if (tab.Get())
    {
        switch (clickEvent)
        {
        case click_section_event::left_down:
            tab->OnLButtonDown(nFlags, point, mOut, er);
            break;
        case click_section_event::left_up:
            tab->OnLButtonUp(nFlags, point, mOut, er);
            break;
        case click_section_event::right_down:
            tab->OnRButtonDown(nFlags, point, mOut, er);
            break;
        case click_section_event::right_up:
            tab->OnRButtonUp(nFlags, point, mOut, er);
        }
    }
}

void IdePageSection::OnMouseMove(UINT nFlags, TPoint point, message_output& mOut, TDataArray<TPage::EventID_Cred>& er)
{
    if (tab.Get())
        tab->OnMouseMove(nFlags, point, mOut, er);
}

void IdePageSection::OnLButtonDblClk(UINT nFlags, TPoint point, message_output& mOut, TDataArray<TPage::EventID_Cred>& er)
{
    if (tab.Get())
        tab->OnLButtonDblClk(nFlags, point, mOut, er);
}

void IdePageSection::OnResize(RECT_F& newLoc, UINT nFlags, TDataArray<TPage::EventID_Cred>& eventAr)
{
    if (this->tab.Get())
        tab->OnResize(newLoc, nFlags, eventAr);
    bounds = newLoc;
}

void IdePageSection::Draw(TrecPointer<TVariable> obj, TrecPointer<TColorBrush> col, float thickness)
{
    if (tab.Get())
        tab->Draw(obj);
}

IdeSection::IdeSection()
{
    bounds = { 0.0f,0.0f,0.0f,0.0f };
}
