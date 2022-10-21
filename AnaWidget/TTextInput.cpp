#include "TTextInput.h"
#include <TInputTextElement.h>
#include "TScrollerPage.h"

TextIncrementControl::TextIncrementControl()
{
	useFloat = true;
	currentVal.f = 0.0f;
	increment.f = 1.0f;
}

void TextIncrementControl::Increment()
{
	if (useFloat)
		currentVal.f += increment.f;
	else
		currentVal.i += increment.i;
}

void TextIncrementControl::Decrement()
{
	if (useFloat)
		currentVal.f -= increment.f;
	else
		currentVal.i -= increment.i;
}

TString TextIncrementControl::GetString()
{
	TString ret;
	ret.Format(useFloat ? L"%f" : L"%d", useFloat ? currentVal.f : currentVal.i);
	return ret;
}

TrecPointer<TTextIntercepter> TTextInput::GetInterceptor()
{
	return text.Get() ? text->GetTextInterceptor() : TrecPointer<TTextIntercepter>();
}

TTextInput::TTextInput(TrecPointer<DrawingBoard> rt, TDataMap<TDataMap<TString>> ta, HWND win) : TGadget(rt, ta)
{
	useNumber = usePassword = passwordPeek = useNumBoxes = showPassword = textLock = false;
	editEnabled = true;
	windowHandle = win;
}

TTextInput::~TTextInput()
{
}

bool TTextInput::onCreate(const RECT_F& loc, TrecPointer<TFileShell> d)
{
	TString valpoint;

	if (attributes.retrieveEntry(L"IsPassword", valpoint) && !valpoint.CompareNoCase(L"true"))
	{
		this->usePassword = true;
		if (attributes.retrieveEntry(L"PasswordPeek", valpoint) && !valpoint.CompareNoCase(L"true"))
			this->passwordPeek = true;
	}
	if (attributes.retrieveEntry(L"IsLocked", valpoint) && !valpoint.CompareNoCase(L"true"))
	{
		this->textLock = true;
	}
	if (attributes.retrieveEntry(L"CanEdit", valpoint) && !valpoint.CompareNoCase(L"false"))
		this->editEnabled = false;

	if (attributes.retrieveEntry(L"IsNumberControl", valpoint) && !valpoint.CompareNoCase(L"true"))
	{
		this->useNumber = true;
		if (attributes.retrieveEntry(L"DrawNumberBoxes", valpoint) && !valpoint.CompareNoCase(L"true"))
		{
			this->useNumBoxes = true;

			if (attributes.retrieveEntry(L"UseInt", valpoint) && !valpoint.CompareNoCase(L"true"))
				this->numberControl.useFloat = false;

			if (attributes.retrieveEntry(L"Increment", valpoint))
			{
				if (this->numberControl.useFloat)
					valpoint.ConvertToFloat(numberControl.increment.f);
				else
					valpoint.ConvertToInt(numberControl.increment.i);
			}
		}
	}

	TGadget::onCreate(loc, d);

	return false;
}

void TTextInput::Draw(TrecPointer<TVariable> object)
{
	TObjectLocker threadLock(&thread);
	if (!isActive)
	{

		return;
	}
	TString curText;
	if (usePassword && !showPassword)
	{
		text->GetText(curText);
		TString passText;
		for (UINT Rust = 0; Rust < curText.GetSize(); Rust++)
			passText.AppendChar(L'\x25cf');
		text->SetText(passText);
	}
	TControl::Draw(object);

	if (passwordPeek)
	{
		ELLIPSE_F passwordPeek_outer, passwordPeek_inner;
		passwordPeek_outer.point = VERTEX_2D{ DxLocation.left + ((DxLocation.right - DxLocation.left) / 2), DxLocation.top + ((DxLocation.bottom - DxLocation.top) / 2) };
		passwordPeek_inner.point = passwordPeek_outer.point;

		passwordPeek_outer.x = (DxLocation.right - DxLocation.left) / 2;
		passwordPeek_outer.y = (DxLocation.bottom - DxLocation.top) / 2;

		passwordPeek_inner.x = passwordPeek_outer.x / 3;
		passwordPeek_inner.y = passwordPeek_outer.y / 3;
		brush->DrawEllipse(passwordPeek_outer);
		brush->FillEllipse(passwordPeek_inner);
	}
	else if (useNumBoxes)
	{
		RECT_F topBut = DxLocation;
		topBut.bottom = topBut.bottom - (topBut.bottom - topBut.top) / 2;
		RECT_F botBut = DxLocation;
		botBut.top = botBut.top + (botBut.bottom - botBut.top) / 2;
		brush->DrawRectangle(topBut);
		brush->DrawRectangle(botBut);
	}

	// deal with passwords
	if (usePassword && !showPassword)
		text->SetText(curText);
}

void TTextInput::OnLButtonUp(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& eventAr)
{
	TObjectLocker threadLock(&thread);
	if (DrawingBoard::IsContained(point, area))
	{
		mOut = message_output::mo_positive_override;

		//auto cThis = TrecPointerKey::GetTrecPointerFromSoft<>(tThis);

		//TPage::EventID_Cred cred(R_Message_Type::On_Focus, cThis, TrecPointerKey::GetNewTrecPointerAlt<TTextIntercepter,
		//	TTextFieldIntercepter>(TrecPointerKey::GetTrecSubPointerFromTrec<TControl, TTextField>(cThis)));
		//eventAr.push_back(cred);

		TControl::OnLButtonUp(nFlags, point, mOut, eventAr);
	}
	showPassword = false;

	text->OnCLickUp(point);
}

void TTextInput::OnLButtonDown(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& eventAr)
{
	TObjectLocker threadLock(&thread);

	if (!editEnabled)
		goto parentCall;

	if (!text->OnCLickDown(point))
		return;

parentCall:
	if (DrawingBoard::IsContained(point, area))
	{
		bool textChange = false;
		if (passwordPeek)
		{
			ELLIPSE_F passwordPeek_outer;
			passwordPeek_outer.point = VERTEX_2D{ DxLocation.left + ((DxLocation.right - DxLocation.left) / 2), DxLocation.top + ((DxLocation.bottom - DxLocation.top) / 2) };

			passwordPeek_outer.x = (DxLocation.right - DxLocation.left) / 2;
			passwordPeek_outer.y = (DxLocation.bottom - DxLocation.top) / 2;
			showPassword = DrawingBoard::IsContained(point, passwordPeek_outer);

		}
		else if (useNumBoxes)
		{
			RECT_F topBut = DxLocation;
			topBut.bottom = topBut.bottom - (topBut.bottom - topBut.top) / 2;
			RECT_F botBut = DxLocation;
			botBut.top = botBut.top + (botBut.bottom - botBut.top) / 2;
			if (useNumBoxes && DrawingBoard::IsContained(point, topBut))
			{
				numberControl.Increment();
				textChange = true;
			}
			else if (useNumBoxes && DrawingBoard::IsContained(point, botBut))
			{
				textChange = true;
				numberControl.Decrement();
			}
		}

		TString methodId(HasEvent(R_Message_Type::On_Text_Change));
		if (textChange && (methodId.GetSize()))
		{
			this->args.Reset();
			this->args.eventType = R_Message_Type::On_Text_Change;
			this->args.methodID = methodId;
			this->args.isClick = true;
			this->args.isLeftClick = true;
			EventID_Cred cred(R_Message_Type::On_Text_Change, TrecPointerKey::TrecFromSoft<>(self));
			cred.args = TrecPointerKey::GetNewTrecPointer<EventArgs>(this->args);

			eventAr.push_back(cred);
		}
	}
	TControl::OnLButtonDown(nFlags, point, mOut, eventAr);
}

void TTextInput::OnMouseMove(UINT nFlags, TPoint point, message_output& mOut, TDataArray<EventID_Cred>& eventAr)
{
	TObjectLocker threadLock(&thread);
	if (text->OnMouseMove(point))
		TControl::OnMouseMove(nFlags, point, mOut, eventAr);
}

void TTextInput::OnResize(RECT_F& newLoc, UINT nFlags, TDataArray<EventID_Cred>& eventAr)
{
	TObjectLocker threadLock(&thread);
	TGadget::OnResize(newLoc, nFlags, eventAr);

	if (text.Get())
	{
		float idealHeight = 0.0;
		text->GetMinHeight(idealHeight);
		float currentHeight = area.bottom - area.top;
		auto actParent = TrecPointerKey::TrecFromSoft<>(parent);
		if (idealHeight <= currentHeight || !actParent.Get())
			return;

		
		if (!dynamic_cast<TScrollerPage*>(actParent.Get()) && actParent.Get())
		{
			area.bottom += idealHeight;
			actParent->InjectScrollerPage(newLoc, area, TrecPointerKey::TrecFromSoft<>(self));
		}
	}
}

bool TTextInput::SetNumericText(float f)
{
	if (!useNumber)
		return false;
	numberControl.currentVal.f = f;
	if (!numberControl.useFloat)
	{
		numberControl.useFloat = true;
		numberControl.increment.f = static_cast<float>(numberControl.increment.i);
	}
}

void TTextInput::SetText(const TString& text)
{
	if (useNumber)
		return;
	if (!this->text.Get())
		SetUpTextElement();
	if (!this->text.Get())
		return;
	this->text->SetText(text);
}

TString TTextInput::GetText()
{
	TString ret;
	if (!text.Get())
		return ret;
	text->GetText(ret);
	return ret;
}

void TTextInput::LockText(bool doLock)
{
	textLock = doLock;
	if (text.Get())
		text->LockText(doLock);
}

void TTextInput::SetPasswordMode(bool b)
{
	usePassword = b;
}

void TTextInput::SetUpTextElement()
{
	TextFormattingDetails details;

	TString locale(L"en-us"), font(L"ariel");

	TString actText;

	tc_line_spacing hAlign = tc_line_spacing::center;
	tc_text_spacing vAlign = tc_text_spacing::center;

	TString valpoint;

	TColor fontColor;
	if (attributes.retrieveEntry(TString(L"Caption"), valpoint))
	{
		actText.Set(valpoint);
	}

	if (attributes.retrieveEntry(TString(L"CaptionLocale"), valpoint))
	{
		locale.Set(valpoint);
	}

	if (attributes.retrieveEntry(TString(L"Font"), valpoint))
	{
		font.Set(valpoint);
	}
	if (attributes.retrieveEntry(TString(L"FontSize"), valpoint))
	{
		valpoint.ConvertToFloat(details.fontSize);
	}
	if (attributes.retrieveEntry(TString(L"HorizontalAlignment"), valpoint))
	{
		hAlign = convertStringToTextAlignment(valpoint);
	}
	if (attributes.retrieveEntry(TString(L"VerticalAlignment"), valpoint))
	{
		vAlign = convertStringToParagraphAlignment(valpoint);
	}

	if (attributes.retrieveEntry(TString(L"FontColor"), valpoint))
	{
		bool b = false;
		fontColor.SetColor(TColor::GetColorFromString(valpoint, b));
	}
	else
		fontColor.SetColor("black");

	// Commented out code (in case gradient colors should be added

	//valpoint = att->retrieveEntry(TString(L"TextGrad"));
	//if (valpoint.Get())
	//{
	//	if (!text1.Get())
	//		text1 = TrecPointerKey::GetNewTrecPointer<TText>(drawingBoard, (this));
	//	float entry = 0.0f;
	//	valpoint->ConvertToFloat(entry);
	//	//text1->secondColor = true;
	//	UINT gradCount = 0;
	//	if (gradCount = text1->stopCollection.GetGradientCount())
	//		text1->stopCollection.SetPositionAt(entry, gradCount - 1);
	//}


	//valpoint = att->retrieveEntry(TString(L"TextGradMode"));
	//if (valpoint.Get())
	//{
	//	if (!text1.Get())
	//		text1 = TrecPointerKey::GetNewTrecPointer<TText>(drawingBoard, (this));
	//	if (valpoint->Compare(L"Radial"))
	//		text1->useRadial = true;
	//}

	// See if have text. If we do, then set up text element

	details.defaultTextColor = fontColor;

	text = TrecPointerKey::GetNewSelfTrecPointerAlt<TTextElement, TInputTextElement>(drawingBoard, windowHandle);

	text->SetBounds(area);
	text->SetBasicFormatting(details);
	text->SetHorizontalAllignment(hAlign);
	text->SetVerticalAllignment(vAlign);
	text->SetText(actText);
}
