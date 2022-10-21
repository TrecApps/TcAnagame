#include "TLayout.h"
#include "TScrollerPage.h"

LayoutSpace::LayoutSpace()
{
	isFlex = false;
	space = actualSpace = 0;
}

LayoutSpace::LayoutSpace(const LayoutSpace& copy)
{
	isFlex = copy.isFlex;
	space = copy.space;
	actualSpace = copy.actualSpace;
}

bool TLayout::onCreate(const RECT_F& loc, TrecPointer<TFileShell> d)
{
	TString valpoint;
	if ((attributes.retrieveEntry(L"IsGallery", valpoint) && !valpoint.CompareNoCase(L"true")) || (attributes.retrieveEntry(L"IsStack", valpoint) && !valpoint.CompareNoCase(L"false")))
		primaryStack = false;

	bool res = TControl::onCreate(loc, d);

	ParseDimensions(primDem, primaryStack ? (area.bottom - area.top) : (area.right - area.left));

	RECT_F chLoc = loc;

	UINT Rust = 0;

	if (!childControls.Size())
		return false;

	do
	{
		auto page = childControls[Rust].control;

		if (Rust)
		{
			if (primaryStack)
			{
				chLoc.top = chLoc.bottom;
				chLoc.bottom += primDem[Rust].actualSpace;
			}
			else
			{
				chLoc.left = chLoc.right;
				chLoc.right += primDem[Rust].actualSpace;
			}
		}
		else
		{
			if (primaryStack)
				chLoc.bottom = chLoc.top + this->primDem[0].actualSpace;
			else
				chLoc.right = chLoc.left + this->primDem[0].actualSpace;
		}

		if (dynamic_cast<TControl*>(page.Get()))
		{
			res &= dynamic_cast<TControl*>(page.Get())->onCreate(chLoc, d);
		}
	} while (++Rust < childControls.Size() && Rust < primDem.Size());

	return res;
}

TLayout::TLayout(TrecPointer<DrawingBoard> drawingBoard, TDataMap<TDataMap<TString>> styles) : TRandomLayout(drawingBoard, styles)
{
	primaryStack = true;
	overflowResponse = overflow_layout_approach::ola_contain;
}

TLayout::~TLayout()
{

}

void TLayout::Draw(TrecPointer<TVariable> object)
{
	TRandomLayout::Draw(object);
}

void TLayout::OnResize(RECT_F& newLoc, UINT nFlags, TDataArray<EventID_Cred>& eventAr)
{
	TControl::OnResize(newLoc, nFlags, eventAr);
	UINT cSize = primaryStack ? (newLoc.bottom - newLoc.top) :
		(newLoc.right - newLoc.left);

	if (ParseDimensions(primDem, cSize))
	{
		// To-Do: Manage Overflow

	}
	if (childControls.Size() && primDem.Size())
	{
		RECT_F cLoc = newLoc;
		if (primaryStack)
		{
			cLoc.bottom = cLoc.top + primDem[0].actualSpace;
			if (childControls[0].control.Get())
				childControls[0].control->OnResize(cLoc, nFlags, eventAr);
			for (UINT Rust = 1; Rust < childControls.Size() && Rust < primDem.Size(); Rust++)
			{
				cLoc.top = cLoc.bottom;
				cLoc.bottom += primDem[Rust].actualSpace;


				if (childControls[Rust].control.Get())
					childControls[Rust].control->OnResize(cLoc, nFlags, eventAr);
			}
		}
		else
		{
			cLoc.right = cLoc.left + primDem[0].actualSpace;
			if (childControls[0].control.Get())
				childControls[0].control->OnResize(cLoc, nFlags, eventAr);
			for (UINT Rust = 1; Rust < childControls.Size() && Rust < primDem.Size(); Rust++)
			{
				cLoc.left = cLoc.right;
				cLoc.right += primDem[Rust].actualSpace;

				if (childControls[Rust].control.Get())
					childControls[Rust].control->OnResize(cLoc, nFlags, eventAr);
			}
		}
	}

}

bool TLayout::OnScroll(bool, const TPoint& point, const TPoint& direction, TDataArray<EventID_Cred>& args)
{
	return TRandomLayout::OnScroll(false, point, direction, args);
}

bool TLayout::AddPage(TrecPointer<TPage> page, UINT row, UINT col, bool doOverride)
{
	UINT index = primaryStack ? row : col;
	UINT sIndex = primaryStack ? col : row;
	if (sIndex & 0x80000000)
		sIndex = 0;

	if (sIndex || index >= childControls.Size())
		return false;

	if (childControls[index].control.Get() && !doOverride)
		return false;

	childControls[index].control = page;
	if (page.Get())
		page->SetParentPage(TrecPointerKey::TrecFromSoft<>(self));
	return true;
}

bool TLayout::RemovePage(UINT row, UINT col)
{
	UINT index = primaryStack ? row : col;
	UINT sIndex = primaryStack ? col : row;

	if (sIndex || index >= childControls.Size())
		return false;

	childControls[index].control.Nullify();
	return true;
}

void TLayout::GetControlCapacity(UINT& row, UINT& col)
{
	row = primaryStack ? primDem.Size() : 1;
	col = primaryStack ? 1 : primDem.Size();
}

TrecPointer<TPage> TLayout::GetPage(UINT row, UINT col)
{
	UINT index = primaryStack ? row : col;
	UINT sIndex = primaryStack ? col : row;

	if (sIndex || index >= childControls.Size())
		return TrecPointer<TPage>();

	return childControls[index].control;
}

int TLayout::AddRow(UINT space, bool isFlex)
{
	// If we're not going by rows, don't do anything
	if (!primaryStack)
		return -1;
	LayoutSpace layoutSpace;
	layoutSpace.space = layoutSpace.actualSpace = space;
	layoutSpace.isFlex = isFlex;

	ChildControl cc;
	cc.col = 0;
	cc.row = childControls.Size();

	this->childControls.push_back(cc);

	return primDem.push_back(layoutSpace);
}

int TLayout::AddCol(UINT space, bool isFlex)
{
	if (primaryStack)
		return -1;
	LayoutSpace layoutSpace;
	layoutSpace.space = layoutSpace.actualSpace = space;
	layoutSpace.isFlex = isFlex;

	ChildControl cc;
	cc.row = 0;
	cc.col = childControls.Size();

	this->childControls.push_back(cc);
	return primDem.push_back(layoutSpace);
}

void TLayout::InjectScrollerPage(const RECT_F& bounds, const RECT_F& needs, TrecPointer<TPage> page)
{
	if (!page.Get())
		return;

	bool found = false;
	UINT target = 0;
	for (UINT Rust = 0; Rust < childControls.Size(); Rust++)
	{
		if (childControls[Rust].control.Get() == page.Get())
		{
			found = true;
			target = Rust;
		}
	}
	if (!found) return;

	if (overflowResponse == overflow_layout_approach::ola_contain)
	{
		TrecPointer<TPage> scroller = TrecPointerKey::GetNewSelfTrecPointerAlt<TPage, TScrollerPage>(drawingBoard, page);
		TDataArray<EventID_Cred> cred;
		auto b = bounds;
		childControls[target].control = scroller;
		scroller->OnResize(b, 0, cred);
	}
	else if (overflowResponse == overflow_layout_approach::ola_vertical)
	{
		//if(!this->primaryStack)
	}
}

bool TLayout::ParseDimensions(TDataArray<LayoutSpace>& dimension, UINT totalSpace)
{
	UINT totalFixed = 0, totalFlex = 0;
	for (UINT Rust = 0; Rust < dimension.Size(); Rust++)
	{
		if (dimension[Rust].isFlex)
			totalFlex += dimension[Rust].space;
		else
			totalFixed += dimension[Rust].space;
	}

	if (totalFixed > totalSpace)
		return true;

	if (totalFixed == totalSpace && totalFlex)
	{
		for (UINT Rust = 0; Rust < dimension.Size(); Rust++)
		{
			dimension[Rust].actualSpace = dimension[Rust].space;
		}
		return true;
	}
	// Avoid Divide by 0 and return, nothing to do
	if (!totalFlex)
		return false;

	float flexRatio = static_cast<float>(totalSpace - totalFixed) / static_cast<float>(totalFlex);

	for (UINT Rust = 0; Rust < dimension.Size(); Rust++)
	{
		if (dimension[Rust].isFlex)
			dimension[Rust].actualSpace = static_cast<float>(dimension[Rust].space) * flexRatio;
	}

	return false;
}
