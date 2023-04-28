#include "TSwitchControl.h"


/**
 * Class: TabBar::TabBarHolder
 * Purpose: Provides the Tab Bar with the means to send the contents of the tab to which-ever control is in charge of hosting it
 */
class TabBarSwitchControlHolder : public TabBar::TabBarHolder
{
    friend class TSwitchControl;
public:
    /**
     * Method: TabBarHolder::SetView
     * Purpose: Called by TabBar when a new Tab is selected
     * Parameters: TrecPointer<TPage> page - the content to display
     *              bool onNew - Relevent if first parameter is null, true if caaused by a click on the new Tab-Tab
     * Returns: void
     *
     * Attributes: abstract
     */
    virtual void SetView(TrecPointer<TPage> page, bool onNew = false) override
    {
        TrecPointer<TSwitchControl> actControl = TrecPointerKey::TrecFromSoft<>(switchControl);
        actControl->SetView(page);
    }

    /**
     * Method: TabBarHolder::RemoveView
     * Purpose: Called by TabBar when a tab Requests to be deleted
     * Parameters: TrecPointer<TPage> page - the content to remove
     * Returns: void
     *
     * Attributes: abstract
     */
    virtual void RemoveView(TrecPointer<TPage> page) override
    {
        TrecPointer<TSwitchControl> actControl = TrecPointerKey::TrecFromSoft<>(switchControl);
        actControl->RemovePage(page);
    }
    /**
     * Method: TabBarHolder::GetContentSpace
     * Purpose: Retrieves the space where the content should go
     * Parameters: void
     * Returns: RECT_F - the area of the content
     *
     * Attributes: abstract
     */
    virtual RECT_F GetContentSpace() override {
        TrecPointer<TSwitchControl> actControl = TrecPointerKey::TrecFromSoft<>(switchControl);
        auto ret = actControl->area;

        auto retAdjust = actControl->GetTabBar()->GetArea();

        if (ret.top == retAdjust.top && ret.bottom)
            ret.top = retAdjust.bottom;
        else if (ret.bottom == retAdjust.bottom && ret.bottom)
            ret.bottom = retAdjust.top;
        return ret;
    }

protected:
    /**
     * The Link to the Switch Control
     */
    TrecPointerSoft<TSwitchControl> switchControl;
};


TrecPointer<TabBar> TSwitchControl::GetTabBar()
{
    return tabBar;
}

TSwitchControl::TSwitchControl(TrecPointer<DrawingBoard> drawingBoard, TDataMap<TDataMap<TString>> styles)
    : TRandomLayout(drawingBoard, styles)
{
    tabBar =TrecPointerKey::ConvertPointer<TPage, TabBar>( TrecPointerKey::GetNewSelfTrecPointerAlt<TPage, TabBar>(drawingBoard) );
    tabShow = 1;
    tabHeight = 30;
    this->isChildLeftClicked = false;
}

bool TSwitchControl::onCreate(const RECT_F& loc, TrecPointer<TFileShell> d)
{
    TrecPointer<TabBar::TabBarHolder> holder =
        TrecPointerKey::GetNewTrecPointerAlt<TabBar::TabBarHolder, TabBarSwitchControlHolder>();

    TrecPointer<TabBarSwitchControlHolder> actHolder = TrecPointerKey::ConvertPointer<TabBar::TabBarHolder, TabBarSwitchControlHolder>(holder);

    childSpace = loc;

    auto switchPointer = TrecPointerKey::ConvertPointer<TPage, TSwitchControl>(
        TrecPointerKey::TrecFromSoft(self)
        );

    actHolder->switchControl = TrecPointerKey::SoftFromTrec<TSwitchControl>(
        switchPointer);
    tabBar->SetHolder(holder);

    TString valpoint;
    if (attributes.retrieveEntry(L"TabsPresent", valpoint))
    {
        if (!valpoint.Compare(L"false"))
            this->tabShow = 0;
        else if (!valpoint.Compare(L"bottom"))
            this->tabShow = 2;
    }

    if (attributes.retrieveEntry(L"TabHeight", valpoint))
    {
        int v = 0;
        if (!valpoint.ConvertToInt(v))
            this->tabHeight = v;

    }// End of valpoint if statement for "TabHeight" and code for creating tabs

    if (attributes.retrieveEntry(L"StackLimit", valpoint))
    {
        int v = 0;
        if (!valpoint.ConvertToInt(v))
            this->maxStack = v;

    }

    RECT_F tabsLoc = childSpace;

    TDataArray<EventID_Cred> cred;
    TDataArray<EventArgs> args;

    switch (tabShow)
    {
    case 1:
        childSpace.top += tabHeight;
        tabsLoc.bottom = childSpace.top;
        TRandomLayout::onCreate(loc, d);
        tabBar->OnResize(tabsLoc, 0, cred);
        break;
    case 2:
        childSpace.bottom -= tabHeight;
        tabsLoc.top = childSpace.bottom;
        TRandomLayout::onCreate(loc, d);
        tabBar->OnResize(tabsLoc, 0, cred);
        break;
    default:
        TRandomLayout::onCreate(loc, d);
    }

    TRandomLayout::onCreate(loc, d);

    UINT unknownCount = 1;
    for (UINT Rust = 0; Rust < childControls.Size(); Rust++)
    {
        auto control = childControls[Rust];
        if (!control.control.Get())
        {
            childControls.RemoveAt(Rust--);
            continue;
        }

        TString tabName;
        tabName.Format(L"Unknown %d", unknownCount);
        bool updated = false;
        if (dynamic_cast<TControl*>(control.control.Get()))
        {
            TString s(dynamic_cast<TControl*>(control.control.Get())->GetID());
            if (s.GetSize())
            {
                tabName.Set(s);
                updated = true;
            }
        }
        if (!updated)
        {
            TString s(control.control->GetName());
            if (s.GetSize())
                tabName.Set(s);
            else
                unknownCount++;
        }
        tabBar->AddNewTab(tabName, control.control, false);
    }

    UINT start = 0;

    if (attributes.retrieveEntry(L"InitialTab", valpoint))
    {
        TString::ConvertStringToUint(valpoint, start);
    }

    auto s = tabBar->GetTabAt(start, true);

    if (s.Get())
        pageStack.push_back(s);

    return false;
}

void TSwitchControl::Draw(TrecPointer<TVariable> object)
{
    TControl::Draw(object);
    UINT pss = pageStack.Size();
    if (pss)
    {
        auto top = pageStack[pss - 1];
        if (top.Get())
            top->Draw(object);
        else
            pageStack.RemoveAt(pss - 1);
    }

    if (tabBar.Get() && tabShow)
    {
        tabBar->Draw(object);
    }
}

void TSwitchControl::OnRButtonUp(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& cred)
{
    if (tabBar.Get() && tabShow)
    {
        tabBar->OnRButtonUp(nFlags, point, mOut, cred);
    }

    if (mOut == message_output::mo_negative)
    {
        UINT pss = pageStack.Size();
        if (pss)
        {
            auto top = pageStack[pss - 1];
            if (top.Get())
                top->OnRButtonUp(nFlags, point, mOut, cred);
            else
                pageStack.RemoveAt(pss - 1);
        }
    }
}

void TSwitchControl::OnRButtonDown(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& cred)
{
    if (tabBar.Get() && tabShow)
    {
        tabBar->OnRButtonDown(nFlags, point, mOut, cred);
    }

    if (mOut == message_output::mo_negative)
    {
        UINT pss = pageStack.Size();
        if (pss)
        {
            auto top = pageStack[pss - 1];
            if (top.Get())
                top->OnRButtonDown(nFlags, point, mOut, cred);
            else
                pageStack.RemoveAt(pss - 1);
        }
    }
}

void TSwitchControl::OnLButtonUp(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& cred)
{
    if (tabBar.Get() && tabShow)
    {
        tabBar->OnLButtonUp(nFlags, point, mOut, cred);
    }

    bool shouldFocus = isChildLeftClicked && DrawingBoard::IsContained(point, childSpace);

    if (mOut == message_output::mo_negative)
    {
        UINT pss = pageStack.Size();
        if (pss)
        {
            auto top = pageStack[pss - 1];
            if (top.Get())
            {
                top->OnLButtonUp(nFlags, point, mOut, cred);

                if (shouldFocus)
                    drawingBoard->SetFocusObject(TrecPointerKey::ConvertPointer<TPage, TObject>(top));
            }
            else
                pageStack.RemoveAt(pss - 1);
        }
    }

    this->isChildLeftClicked = true;
}

void TSwitchControl::OnMouseMove(UINT nFlags, TPoint point, message_output& mOut, TDataArray<EventID_Cred>& cred)
{
    if (tabBar.Get() && tabShow)
    {
        tabBar->OnMouseMove(nFlags, point, mOut, cred);
    }

    if (mOut == message_output::mo_negative)
    {
        UINT pss = pageStack.Size();
        if (pss)
        {
            auto top = pageStack[pss - 1];
            if (top.Get())
                top->OnMouseMove(nFlags, point, mOut, cred);
            else
                pageStack.RemoveAt(pss - 1);
        }
    }
}

void TSwitchControl::OnLButtonDblClk(UINT nFlags, TPoint point, message_output& mOut, TDataArray<EventID_Cred>& args)
{

    if (tabBar.Get() && tabShow)
    {
        tabBar->OnLButtonDblClk(nFlags, point, mOut, args);
    }

    if (mOut == message_output::mo_negative)
    {
        UINT pss = pageStack.Size();
        if (pss)
        {
            auto top = pageStack[pss - 1];
            if (top.Get())
                top->OnLButtonDblClk(nFlags, point, mOut, args);
            else
                pageStack.RemoveAt(pss - 1);
        }
    }
}

void TSwitchControl::OnLButtonDown(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& cred)
{
    if (tabBar.Get() && tabShow)
    {
        tabBar->OnLButtonDown(nFlags, point, mOut, cred);
    }

    if (mOut == message_output::mo_negative)
    {
        UINT pss = pageStack.Size();
        if (pss)
        {
            auto top = pageStack[pss - 1];
            if (top.Get())
                top->OnLButtonDown(nFlags, point, mOut, cred);
            else
                pageStack.RemoveAt(pss - 1);
        }
    }

    if (DrawingBoard::IsContained(point, this->childSpace))
    {
        this->isChildLeftClicked = true;
        EventID_Cred id;
        id.control = TrecPointerKey::TrecFromSoft<>(self);
        id.eventType = R_Message_Type::On_L_Button_Down;
        cred.push_back(id);
    }
}

void TSwitchControl::OnResize(RECT_F& newLoc, UINT nFlags, TDataArray<EventID_Cred>& cred)
{
    RECT_F tabsLoc = newLoc;
    childSpace = newLoc;

    switch (tabShow)
    {
    case 1:
        childSpace.top += tabHeight;
        tabsLoc.bottom = childSpace.top;
        tabBar->OnResize(tabsLoc, 0, cred);
        break;
    case 2:
        childSpace.bottom -= tabHeight;
        tabsLoc.top = childSpace.bottom;
        tabBar->OnResize(tabsLoc, 0, cred);
        break;
    }

    TRandomLayout::OnResize(childSpace, nFlags, cred);
    for (UINT Rust = 0; Rust < pageStack.Size(); Rust++) {
        auto page = pageStack[Rust].Get();
        if (page)
            page->OnResize(childSpace, nFlags, cred);
    }
    area = newLoc;
}

bool TSwitchControl::OnScroll(bool b, const TPoint& point, const TPoint& direction, TDataArray<EventID_Cred>& args)
{
    bool ret = true;
    if (tabBar.Get() && tabShow)
    {
        ret &= tabBar->OnScroll(b, point, direction, args);
    }


    UINT pss = pageStack.Size();
    if (pss)
    {
        auto top = pageStack[pss - 1];
        if (top.Get())
            ret &= top->OnScroll(b, point, direction, args);
        else
            pageStack.RemoveAt(pss - 1);
    }
    return ret;
}

void TSwitchControl::SetView(TrecPointer<TPage> page)
{
    while (maxStack && pageStack.Size() >= maxStack)
    {
        bool removed = false;
        for (UINT Rust = 0; Rust < pageStack.Size(); Rust++)
        {
            if (page.Get() == pageStack[Rust].Get())
            {
                pageStack.RemoveAt(Rust);
                removed = true;
                break;
            }
        }

    }
    TrecPointer<TPage> pagePointer = dynamic_cast<TabBar::Tab*>(page.Get()) ? dynamic_cast<TabBar::Tab*>(page.Get())->GetContent() : page;
    if (pagePointer.Get()) {
        pageStack.push_back(pagePointer);
        TDataArray<EventID_Cred> cred;
        pagePointer->OnResize(this->childSpace, 0, cred);

        drawingBoard->SetFocusObject(TrecPointerKey::ConvertPointer<TPage, TObject>(pagePointer));
    }
}

void TSwitchControl::RemovePage(TrecPointer<TPage> page)
{
    for (UINT Rust = 0; Rust < pageStack.Size(); Rust++)
    {
        if (page.Get() == pageStack[Rust].Get())
        {
            pageStack.RemoveAt(Rust--);
            continue;
        }
    }
}
