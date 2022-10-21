#include "TCheckBox.h"

TCheckBox::TCheckBox(TrecPointer<DrawingBoard> rt, TDataMap<TDataMap<TString>> ta) : TGadget(rt, ta)
{
	isChecked = false;
}

TCheckBox::~TCheckBox()
{
}

bool TCheckBox::onCreate(const RECT_F& loc, TrecPointer<TFileShell> d)
{
	TGadget::onCreate(loc, d);

	if (!text.Get())
	{
		text = TrecPointerKey::GetNewTrecPointer<TTextElement>(drawingBoard);
		text->SetText(L"Check-Box");
	}
	auto tempLoc = text->GetBounds();
	tempLoc.left += bSize;
	text->SetBounds(tempLoc);
	return true;
}

void TCheckBox::Draw(TrecPointer<TVariable> object)
{
	TObjectLocker lock(&thread);
	if (!isActive)
	{
		return;
	}
	TControl::Draw(object);

	brush->DrawRectangle(DxLocation);
	if (this->isChecked)
	{
		brush->DrawLine(TPoint(DxLocation.left, DxLocation.top), TPoint(DxLocation.right, DxLocation.bottom));
		brush->DrawLine(TPoint(DxLocation.right, DxLocation.top), TPoint(DxLocation.left, DxLocation.bottom));
	}
}

bool TCheckBox::IsChecked()
{
	TObjectLocker lock(&thread);
	return isChecked;
}

void TCheckBox::OnLButtonUp(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& eventAr)
{
	if (this->isLeftClicked && DrawingBoard::IsContained(point, area))
	{
		isChecked = !isChecked;
		EventID_Cred cred(R_Message_Type::On_check, TrecPointerKey::TrecFromSoft<>(self));
		TString checkIndex = HasEvent(R_Message_Type::On_check);
		if (checkIndex.GetSize())
		{
			this->args.Reset();
			this->args.point = point;
			this->args.eventType = R_Message_Type::On_check;
			this->args.positive = isChecked;
			if (text.Get())
				text->GetText(this->args.text);
			this->args.methodID = checkIndex;

			cred.args = TrecPointerKey::GetNewTrecPointer<EventArgs>(this->args);
		}
		eventAr.push_back(cred);
	}
}



void TCheckBox::OnResize(RECT_F& newLoc, UINT nFlags, TDataArray<EventID_Cred>& eventAr)
{
	TGadget::OnResize(newLoc, nFlags, eventAr);

	if (text.Get())
	{
		auto tempLoc = text->GetBounds();
		tempLoc.left += bSize;
		text->SetBounds(tempLoc);
	}
}
