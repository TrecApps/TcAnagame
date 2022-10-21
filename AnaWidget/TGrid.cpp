#include "TGrid.h"


bool TGrid::onCreate(const RECT_F& loc, TrecPointer<TFileShell> d)
{
    bool res = TControl::onCreate(loc, d);

    ParseDimensions(primDem, primaryStack ? (area.bottom - area.top) : (area.right - area.left));
    ParseDimensions(secDem, primaryStack ? (area.right - area.left) : (area.bottom - area.top));

    RECT_F chLoc = loc;

    UINT Rust = 0;

    if (!childControls.Size())
        return false;

    for (UINT Rust = 0; Rust < childControls.Size(); Rust++)
    {
        chLoc = GetLocationBySection(childControls[Rust].row, childControls[Rust].col);
        auto page = childControls[Rust].control;
        if (dynamic_cast<TControl*>(page.Get()))
        {
            res &= dynamic_cast<TControl*>(page.Get())->onCreate(chLoc, d);
        }
    }

    return res;
}

TGrid::TGrid(TrecPointer<DrawingBoard> drawingBoard, TDataMap<TDataMap<TString>> styles) : TLayout(drawingBoard, styles)
{
}

TGrid::~TGrid()
{
}

void TGrid::Draw(TrecPointer<TVariable> object)
{
    TRandomLayout::Draw(object);
}

void TGrid::OnResize(RECT_F& newLoc, UINT nFlags, TDataArray<EventID_Cred>& eventAr)
{
    TControl::OnResize(newLoc, nFlags, eventAr);
    UINT height = newLoc.bottom - newLoc.top;
    UINT width = newLoc.right - newLoc.left;

    bool widthOverflow = ParseDimensions(primDem, primaryStack ? height : width);
    bool heightOverflow = ParseDimensions(secDem, primaryStack ? width : height);

    if (widthOverflow || heightOverflow)
    {
        // To-Do: Handle
    }

    for (UINT Rust = 0; Rust < childControls.Size(); Rust++)
    {
        RECT_F chLoc = GetLocationBySection(childControls[Rust].row, childControls[Rust].col);
        auto page = childControls[Rust].control;
        if (dynamic_cast<TControl*>(page.Get()))
        {
            page->OnResize(chLoc, nFlags, eventAr);
        }
    }
}

bool TGrid::OnScroll(bool, const TPoint& point, const TPoint& direction, TDataArray<EventID_Cred>& args)
{
    return TRandomLayout::OnScroll(false, point, direction, args);
}

bool TGrid::AddPage(TrecPointer<TPage> page, UINT row, UINT col, bool doOverride)
{
    TDataArray<LayoutSpace>* rows = primaryStack ? &primDem : &secDem;
    TDataArray<LayoutSpace>* cols = primaryStack ? &secDem : &primDem;

    if (row >= rows->Size() || col >= cols->Size())
        return false;


    for (UINT Rust = 0; Rust < childControls.Size(); Rust++)
    {

        ChildControl& cc = childControls[Rust];
        if (cc.row == row && cc.col == col)
        {
            if (cc.control.Get() && !doOverride)
                return false;
            childControls[Rust].control = page;
            return true;
        }
    }

    return false;
}

bool TGrid::RemovePage(UINT row, UINT col)
{
    TDataArray<LayoutSpace>* rows = primaryStack ? &primDem : &secDem;
    TDataArray<LayoutSpace>* cols = primaryStack ? &secDem : &primDem;

    if (row >= rows->Size() || col >= cols->Size())
        return false;

    for (UINT Rust = 0; Rust < childControls.Size(); Rust++)
    {
        if (childControls[Rust].col == col && childControls[Rust].row == row)
        {
            childControls[Rust].control.Nullify();
            return true;
        }
    }
    return false;

}

void TGrid::GetControlCapacity(UINT& row, UINT& col)
{
    TDataArray<LayoutSpace>* rows = primaryStack ? &primDem : &secDem;
    TDataArray<LayoutSpace>* cols = primaryStack ? &secDem : &primDem;
    row = rows->Size();
    col = cols->Size();
}

int TGrid::AddRow(UINT space, bool isFlex)
{
    LayoutSpace layoutSpace;
    layoutSpace.space = layoutSpace.actualSpace = space;
    layoutSpace.isFlex = isFlex;

    int ret = primaryStack ? primDem.push_back(layoutSpace) : secDem.push_back(layoutSpace);
    RefreshChildControls(true);
    return ret;
}

int TGrid::AddCol(UINT space, bool isFlex)
{
    LayoutSpace layoutSpace;
    layoutSpace.space = layoutSpace.actualSpace = space;
    layoutSpace.isFlex = isFlex;

    int ret = primaryStack ? secDem.push_back(layoutSpace) : primDem.push_back(layoutSpace);
    RefreshChildControls(true);
    return ret;
}

TrecPointer<TPage> TGrid::GetPage(UINT row, UINT col)
{
    for (UINT Rust = 0; Rust < childControls.Size(); Rust++)
    {
        auto childControl = childControls[Rust];
        if (childControl.row == row && childControl.col == col)
            return childControl.control;
    }
    return TrecPointer<TPage>();
}

RECT_F TGrid::GetLocationBySection(UINT row, UINT col)
{
    TDataArray<LayoutSpace>* rowSpace = primaryStack ? &primDem : &secDem;
    TDataArray<LayoutSpace>* colSpace = primaryStack ? &secDem : &primDem;

    if ((row && row >= rowSpace->Size()) || (col && col >= colSpace->Size()))
        return RECT_F();

    RECT_F ret = area;

    if (rowSpace->Size())
    {
        for (UINT Rust = 0; Rust < row; Rust++)
            ret.top += rowSpace->at(Rust).actualSpace;
        ret.bottom = ret.top + rowSpace->at(row).actualSpace;
    }

    if (colSpace->Size())
    {
        for (UINT Rust = 0; Rust < col; Rust++)
            ret.left += colSpace->at(Rust).actualSpace;
        ret.right = ret.left + colSpace->at(col).actualSpace;
    }
    return ret;
}

void TGrid::RefreshChildControls(bool newRow)
{


    if (newRow)
    {
        int row = (primaryStack ? primDem.Size() : secDem.Size()) - 1;
        if (row < 0) return;
        for (UINT Rust = 0; Rust < (primaryStack ? secDem.Size() : primDem.Size()); Rust++)
        {
            ChildControl cc;
            cc.col = Rust;
            cc.row = row;
            childControls.push_back(cc);
        }
    }
    else
    {
        TDataArray<ChildControl> newControls;
        int col = (primaryStack ? secDem.Size() : primDem.Size()) - 1;
        if (col < 0) return;
        for (UINT Rust = 0; Rust < childControls.Size(); Rust++)
        {
            newControls.push_back(childControls[Rust]);
            if (childControls[Rust].col == col - 1)
            {
                ChildControl cc;
                cc.row = childControls[Rust].row;
                cc.col = col;
                newControls.push_back(cc);
            }
        }
        childControls = newControls;
    }
}
