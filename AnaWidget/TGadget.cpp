#include "TGadget.h"

TGadget::TGadget(TrecPointer<DrawingBoard> rt, TDataMap<TDataMap<TString>> ta) : TControl(rt, ta)
{
	dxColor = TColor("Black");
	thickness = 1.0f;

	bSize = 30;
	ZeroMemory(&DxLocation, sizeof(DxLocation));
}

TGadget::~TGadget()
{
}

bool TGadget::onCreate(const RECT_F& r, TrecPointer<TFileShell> d)
{
	ThreadLock();

	TControl::onCreate(r, d);

	bSize = area.bottom - area.top;
	int height = bSize;

	if (bSize > area.right - area.left)
		bSize = area.right - area.left;

	if (bSize > 30)
		bSize = 30;

	TString valpoint;

	//TrecPointer<TString> valpoint = attributes.retrieveEntry();

	bool setContent = (attributes.retrieveEntry(L"AutoGenContent", valpoint) && !valpoint.CompareNoCase(L"false")) ? false : true;

	if (!this->content.Get() && setContent)
	{
		content = TControlComponent::GetControlContent(drawingBoard, TrecPointerKey::TrecFromSoft<>(self));
		content->onCreate(area);                         // this this isn't covered by the TControl
		// as it didn't exist yet
	}


	if (attributes.retrieveEntry(L"BoxSize", valpoint))
	{

		valpoint.ConvertToInt(bSize);

	}
	int offset = (height - bSize) / 2;
	DxLocation.top = area.top + offset;
	DxLocation.bottom = area.bottom - offset;
	DxLocation.left = area.left;
	DxLocation.right = DxLocation.left + bSize;


	if (drawingBoard.Get())
	{

		brush = TrecPointerKey::ConvertPointer<TBrush, TColorBrush>(drawingBoard->GetSolidColorBrush(TColor(dxColor)));
	}
	ThreadRelease();


	return false;
}

void TGadget::OnResize(RECT_F& newLoc, UINT nFlags, TDataArray<EventID_Cred>& eventAr)
{
	ThreadLock();
	TControl::OnResize(newLoc, nFlags, eventAr);
	int height = area.bottom - area.top;
	int offset = (height - bSize) / 2;
	DxLocation.top = area.top + offset;
	DxLocation.bottom = area.bottom - offset;
	DxLocation.left = area.left;
	DxLocation.right = DxLocation.left + bSize;

	ThreadRelease();
}
