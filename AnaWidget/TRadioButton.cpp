#include "TRadiobutton.h"

TRadioButton::TRadioClass::TRadioClass()
{
}

TRadioButton::TRadioClass::TRadioClass(const TRadioClass& copy)
{
	this->name.Set(copy.name);

	for (UINT Rust = 0; Rust < copy.buttons.Size(); Rust++)
	{
		buttons.push_back(copy.buttons[Rust]);
	}
}

void TRadioButton::TRadioClass::AddButton(TrecPointer<TRadioButton> radioPage)
{
	if (!radioPage.Get())return;
	for (UINT Rust = 0; Rust < buttons.Size(); Rust++)
	{
		auto button = TrecPointerKey::TrecFromSoft<>(buttons[Rust]);
		if (radioPage.Get() == button.Get())
			return;
	}
	buttons.push_back(TrecPointerKey::SoftFromTrec<>(radioPage));
}

UINT TRadioButton::TRadioClass::RemoveButton(TRadioButton* remove)
{
	for (UINT Rust = 0; Rust < buttons.Size(); Rust++)
	{
		auto button = TrecPointerKey::TrecFromSoft<>(buttons[Rust]);
		if (button.Get() == remove)
		{
			buttons.RemoveAt(Rust);
			break;
		}
	}
	return buttons.Size();
}

void TRadioButton::TRadioClass::ClaimClick(TRadioButton* claiment)
{
	for (UINT Rust = 0; Rust < buttons.Size(); Rust++)
	{
		auto button = TrecPointerKey::TrecFromSoft<>(buttons[Rust]);
		if (button.Get() != claiment)
			button.Get()->UnCheck();
	}
}

TDataArray<TrecPointer<TRadioButton::TRadioClass>> radioClasses;

TrecPointer<TRadioButton::TRadioClass> TRadioButton::GetRadioClass(const TString& name)
{
	bool nullFound = false;
	UINT nullIndex = 0;
	for (UINT Rust = 0; Rust < radioClasses.Size(); Rust++)
	{
		if (!radioClasses[Rust].Get() && !nullFound)
		{
			nullFound = true;
			nullIndex = Rust;
			continue;
		}
		if (!name.Compare(radioClasses[Rust]->name))
		{
			return radioClasses[Rust];
		}
	}

	TrecPointer<TRadioButton::TRadioClass> r = TrecPointerKey::GetNewTrecPointer<TRadioButton::TRadioClass>();
	r->name.Set(name);

	if (nullFound)
		radioClasses[nullIndex] = r;
	else
		radioClasses.push_back(r);

	return r;
}

TRadioButton::TRadioButton(TrecPointer<DrawingBoard> rt, TDataMap<TDataMap<TString>> ta) : TGadget(rt, ta)
{
	isChecked = false;
	this->ellBut.x = ellBut.y = 0.0f;
	ellBut.point = { 0.0f, 0.0f };
}

TRadioButton::~TRadioButton()
{
}

bool TRadioButton::onCreate(const RECT_F& loc, TrecPointer<TFileShell> d)
{
	TGadget::onCreate(loc, d);


	if (attributes.retrieveEntry(L"RadioClass", radioClass))
	{
		auto group = GetRadioClass(radioClass);
		group->AddButton(TrecPointerKey::ConvertPointer<TPage, TRadioButton>(TrecPointerKey::TrecFromSoft<>(self)));
	}

	if (text.Get())
	{
		auto tempLoc = text->GetBounds();
		tempLoc.left += bSize;
		text->SetBounds(tempLoc);
	}
	else
	{
		text = TrecPointerKey::GetNewTrecPointer<TTextElement>(drawingBoard);
		text->SetText(L"Radio-Botton");
	}

	ellBut.point = VERTEX_2D{ (DxLocation.right + DxLocation.left) / 2, (DxLocation.top + DxLocation.bottom) / 2 };
	ellBut.x = (DxLocation.right - DxLocation.left) / 2;
	ellBut.y = (DxLocation.bottom - DxLocation.top) / 2;

	return false;
}

void TRadioButton::Draw(TrecPointer<TVariable> object)
{

	TControl::Draw(object);

	if (isActive)
	{
		if (isChecked)
			brush->FillEllipse(ellBut);
		else
			brush->DrawEllipse(ellBut);
	}
}

void TRadioButton::OnLButtonUp(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& eventAr)
{
	if (this->isLeftClicked && DrawingBoard::IsContained(point, area))
	{
		isChecked = true;

		if (radioClass.GetSize())
		{
			auto group = GetRadioClass(radioClass);
			group->ClaimClick(this);
		}
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

void TRadioButton::OnResize(RECT_F& newLoc, UINT nFlags, TDataArray<EventID_Cred>& eventAr)
{
	TGadget::OnResize(newLoc, nFlags, eventAr);

	if (text.Get())
	{
		auto tempLoc = text->GetBounds();
		tempLoc.left += bSize;
		text->SetBounds(tempLoc);
	}
	ellBut.point = VERTEX_2D{ (DxLocation.right + DxLocation.left) / 2, (DxLocation.top + DxLocation.bottom) / 2 };
	ellBut.x = (DxLocation.right - DxLocation.left) / 2;
	ellBut.y = (DxLocation.bottom - DxLocation.top) / 2;
}

void TRadioButton::UnCheck()
{
	TObjectLocker lock(&thread);
	isChecked = false;
}
