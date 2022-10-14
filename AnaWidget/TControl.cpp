#include "TControl.h"
#include <cassert>

typedef struct EventInString
{
	R_Message_Type type;
	TString str;
}EventInString;


EventInString eventsInStringForm[] = {
	{R_Message_Type::On_Char, L"EventOnChar"},
	{R_Message_Type::On_check, L"EventOnCheck"},
	{R_Message_Type::On_Click, L"EventOnClick"},
	{R_Message_Type::On_Click_Release, L"EventOnRelease"},
	{R_Message_Type::On_Click_Release, L"EventOnLeftRelease"},
	{R_Message_Type::On_Flyout, L"EventOnFlyout"},
	{R_Message_Type::On_Focus, L"OnFocus"},
	{R_Message_Type::On_Focus, L"EventOnFocus"},
	{R_Message_Type::On_Hold_Click, L"EventOnHoldClick"},
	{R_Message_Type::On_Hover, L"EventOnHover"},
	{R_Message_Type::On_Hover_Enter, L"EventOnHoverEnter"},
	{R_Message_Type::On_Hover_Leave, L"EventOnHoverLeave"},
	{R_Message_Type::On_LDoubleClick, L"EventOnDblClick"},
	{R_Message_Type::On_Lose_Focus, L"OnLoseFocus"},
	{R_Message_Type::On_Lose_Focus, L"EventOnLoseFocus"},
	{R_Message_Type::On_L_Button_Down, L"EventOnLButtonDown"},
	{R_Message_Type::On_L_Button_Up, L"EventOnLButtonUp"},
	{R_Message_Type::On_radio_change, L"EventOnRadioChange"},
	{R_Message_Type::On_Resized, L"EventOnResize"},
	{R_Message_Type::On_Right_Click, L"EventOnRightClick"},
	{R_Message_Type::On_Right_Release, L"EventOnRightRelease"},
	{R_Message_Type::On_R_Button_Down, L"EventOnRButtonDown"},
	{R_Message_Type::On_R_Button_Up, L"EventOnRButtonUp"},
	{R_Message_Type::On_Scrolled, L"EventOnScrolled"},
	{R_Message_Type::On_Select_Scroller, L"EventOnScrollerSelect"},
	{R_Message_Type::On_sel_change, L"EventOnSelectionChange"},
	{R_Message_Type::On_sel_change, L"EventSelectionChange"},
	{R_Message_Type::On_SubmitDrag, L"EventOnSubmitDrag"},
	{R_Message_Type::On_Text_Change, L"EventOnTextChange"},

};


EventTypeID::EventTypeID(const EventTypeID& copy)
{
	eventType = copy.eventType;
	eventID.Set(copy.eventID);
}

EventTypeID::EventTypeID()
{
	eventType = R_Message_Type::On_Click;
	eventID = -1;
}

EventTypeID::EventTypeID(R_Message_Type type, const TString& id)
{
	eventType = type;
	eventID.Set(id);
}

/**
 * Method:  TControlComponent::TControlComponent
 * Purpose: Constructor
 * Parameters: TrecPointer<DrawingBoard> dbp - Smart Pointer to the Render target to draw against
 *				TControl* tc - the TControl to which the TControlComponent is a party to
 * Returns: New TControlComponent Object
 */
TControlComponent::TControlComponent(TrecPointer<DrawingBoard>rtp, TrecPointer<TPage> parent)
{
	drawingBoard = rtp;
	this->parent = TrecPointerKey::SoftFromTrec<>(parent);

	thickness = 1.0;
	isContent = false;

	if (drawingBoard.Get())
	{
		BuilderFocusBrush = drawingBoard->GetSolidColorBrush(TColor(0x00FF0000));
	}
	else
		BuilderFocusBrush.Nullify();
}

/**
 * Method: TControlComponent:TControlComponent
 * Purpose: Constructor
 * Parameters: TrecPointer<TControlComponent> & rBord - Reference to the Border to copy
 *				TControl* tc_holder - the Control to work for
 * Returns: New TControlComponent Object
 */
TControlComponent::TControlComponent(TrecPointer<TControlComponent>& rBord, TrecPointer<TPage> parent)
{
	if (!rBord.Get())return;

	thickness = rBord->thickness;
	style = rBord->style;
	//stopCollection = rBord->stopCollection;
	drawingBoard = rBord->drawingBoard;

	this->parent = TrecPointerKey::SoftFromTrec<>(parent);
	shape = rBord->shape;

	useRadial = rBord->useRadial;

	this->isContent = rBord->isContent;

	// Alternate shapes
	loci = rBord->loci;
	circle = rBord->circle;
	roundedRect = rBord->roundedRect;
}

/**
 * Method: TControlComponent::~TControlComponent
 * Purpose: Destructor
 * Parameters: void
 * Returns: void
 */
TControlComponent::~TControlComponent()
{
	brush.Nullify();

	BuilderFocusBrush.Nullify();
	image.Nullify();
}

/**
 * Method: TControlComponent::onCreate
 * Purpose: Sets up a basic Rectangular border
 * Parameters: RECT location - the location the Border should draw on
 * Returns: bool - success
 */
bool TControlComponent::onCreate(RECT_F location)
{
	TObjectLocker threadLock(&thread);
	if (!drawingBoard.Get())
	{

		return false;
	}
	brush.Nullify();


	//TrecComPointer<ID2D1GradientStopCollection>::TrecComHolder stopCollRaw;
	shape = TShape::T_Rect;
	loci.bottom = location.bottom;
	loci.left = location.left;
	loci.right = location.right;
	loci.top = location.top;

	ResetBrush();


	return true;
}


/**
 * Method: TControlComponent::onDraw
 * Purpose: Draws the border
 * Parameters: void
 * Returns: void
 */
void TControlComponent::onDraw(RECT_F& f_loc)
{
	TObjectLocker threadLock(&thread);

	TrecPointer<TPage> actParent = TrecPointerKey::TrecFromSoft<>(parent);

	if (drawingBoard.Get() && brush.Get() && actParent.Get())
	{
		bool drawRect = true;

		if (isContent)
		{
			//bool isLayered = false;
			//switch (shape)
			//{
			//case TShape::T_Rect:

			//	break;
			//case TShape::T_Rounded_Rect:
			//	drawingBoard->AddLayer(roundedRect);
			//	isLayered = true;
			//	break;
			//case TShape::T_Ellipse:
			//	circle.point.x = (f_loc.right + f_loc.left) / 2;
			//	circle.point.y = (f_loc.bottom + f_loc.top) / 2;
			//	circle.radiusX = f_loc.right - f_loc.left;
			//	circle.radiusY = f_loc.bottom - f_loc.top;
			//	drawingBoard->AddLayer(circle);
			//	isLayered = true;
			//	break;
			//case TShape::T_Custom_shape:
			//	break;
			//}
			brush->FillRectangle(f_loc);
			if (image.Get())
				image->FillRectangle(f_loc);

			//if (isLayered)
			//	drawingBoard->PopLayer();

		}
		else
		{
			//switch (shape)
			//{
			//case TShape::T_Rect:
				brush->DrawRectangle(f_loc, thickness);
			//	break;
			//case TShape::T_Rounded_Rect:
			//	brush->DrawRoundedRectangle(roundedRect, thickness);
			//	break;
			//case TShape::T_Ellipse:
			//	circle.point.x = (f_loc.right + f_loc.left) / 2;
			//	circle.point.y = (f_loc.bottom + f_loc.top) / 2;
			//	circle.radiusX = f_loc.right - f_loc.left;
			//	circle.radiusY = f_loc.bottom - f_loc.top;
			//	brush->DrawEllipse(circle, thickness);
			//	break;
			//case TShape::T_Custom_shape:
			//	break;
			//}
		}
	}

}

void TControlComponent::setThickness(float f)
{
	TObjectLocker threadLock(&thread);
	thickness = f;

}
void TControlComponent::setOpaquency(float f)
{
	if (f > 1.0000000 || f < 0.0f)
		return;
	TObjectLocker threadLock(&thread);
	if (brush.Get())
	{
		for (UINT Rust = 0; Rust < brush->GetMaxColors(); Rust++)
		{
			TColor color(brush->GetColor(Rust));
			color.SetColor(color.GetRed(), color.GetGreen(), color.GetBlue(), f);
			brush->SetColor(color, Rust);
		}
	}

}

float TControlComponent::getThickness()
{
	TObjectLocker threadLock(&thread);
	float ret = thickness;

	return ret;
}

RECT_F TControlComponent::getLocation()
{
	TObjectLocker threadLock(&thread);

	return loci;
}

void TControlComponent::ShiftHorizontal(int degrees)
{
	TObjectLocker threadLock(&thread);
	loci.left += degrees;
	loci.right += degrees;
}

void TControlComponent::ShiftVertical(int degrees)
{
	TObjectLocker threadLock(&thread);
	loci.top += degrees;
	loci.bottom += degrees;
}


TrecPointer<TColorBrush> TControlComponent::GetBrush()
{
	TObjectLocker threadLock(&thread);

	return brush;
}

TrecPointer<TControlComponent> TControlComponent::GetControlBorder(TrecPointer<DrawingBoard> board, TrecPointer<TPage> parent)
{
	if (!board.Get() || !parent.Get())
		return TrecPointer<TControlComponent>();
	return TrecPointerKey::GetNewTrecPointer<TControlComponent>(board, parent);
}

TrecPointer<TControlComponent> TControlComponent::GetControlContent(TrecPointer<DrawingBoard> board, TrecPointer<TPage> parent)
{
	TrecPointer<TControlComponent> ret = GetControlBorder(board, parent);
	if (ret.Get())
		ret->isContent = true;
	return ret;
}

void TControlComponent::ResetBrush()
{
	TObjectLocker threadLock(&thread);
	if (!drawingBoard.Get())
	{

		return;
	}
	if (stopCollection.GetGradientCount() > 1)
	{

		//if (useRadial)
		//{
		//	brush = drawingBoard->GetBrush(stopCollection, TPoint(loci.left, loci.top),
		//		TPoint(loci.right, loci.bottom),
		//		loci.right - loci.left, loci.bottom - loci.top);
		//}
		//else
		//{
		//	brush = drawingBoard->GetBrush(stopCollection, TPoint(loci.left, loci.top),
		//		TPoint(loci.right, loci.bottom));
		//}

	}
	else if (stopCollection.GetGradientCount() == 1)
	{
		brush = TrecPointerKey::ConvertPointer<TBrush, TColorBrush>(drawingBoard->GetSolidColorBrush(TColor(stopCollection.GetColorAt(0))));
	}
	else
	{
		brush = TrecPointerKey::ConvertPointer<TBrush, TColorBrush>(drawingBoard->GetSolidColorBrush(TColor()));
	}

}

void TControl::AddAttribute(const TString& att, const TString& value)
{
	this->attributes.addEntry(att, value);
}

TControl::TControl(TrecPointer<DrawingBoard> drawingBoard, TDataMap<TDataMap<TString>> styles) : TPage(drawingBoard)
{
	isActive = true;
	mState = messageState::normal;
	overrideParent = true;
	arrayID = -1;
	//controlTransform = D2D1::Matrix3x2F::Identity();
	shape = TShape::T_Rect;
	area = margin = bounds = { 0,0,0,0 };
	isRightClicked = isLeftClicked = isMouseIn = false;
	fixedWidth = fixedHeight = false;

	this->styles = styles;
}

/**
 * Method: TControl::setActive
 * Purpose: Enables controls to be either active (default) or inactive (they don't draw or respond to events)
 * Parameters: bool act - whether control should be active or not
 * Returns: void
 */
void TControl::setActive(bool act)
{
	TObjectLocker threadLock(&thread);
	isActive = act;

}

/**
 * Method: TControl::GetActive
 * Purpose: Reports whether the control is 'active' or not
 * Parameters: void
 * Returns: bool - whether the control is active or not
 */
bool TControl::GetActive()
{
	TObjectLocker threadLock(&thread);
	bool ret = isActive;

	return ret;
}

bool TControl::onCreate(const RECT_F& loc, TrecPointer<TFileShell> d)
{
	TObjectLocker threadLock(&thread);

	InspectEventAttributes();

	bounds = loc;
	TString valpoint;

	OnCreateSize();

	if (attributes.retrieveEntry(L"ArrayID", valpoint))
	{
		valpoint.ConvertToInt(arrayID);
	}

	//if (attributes.retrieveEntry(L"Shape", valpoint))
	//{
	//	if (valpoint.Compare(L"Ellipse"))
	//	{
	//		shape = TShape::T_Ellipse;
	//		ellipse.point = TPoint((area.right + area.left) / 2, (area.top + area.bottom) / 2);
	//		ellipse.radiusX = (area.right - area.left) / 2;
	//		ellipse.radiusY = (area.bottom - area.top) / 2;
	//	}
	//	else if (valpoint.Compare(L"RoundedRectangle"))
	//	{
	//		shape = TShape::T_Rounded_Rect;
	//		float xRound = (area.left - area.right) / 10;
	//		float yRound = (area.bottom - area.top) / 10;
	//		if (attributes.retrieveEntry(L"RoundedRectX", valpoint))
	//		{
	//			valpoint.ConvertToFloat(xRound);
	//		}
	//		if (attributes.retrieveEntry(L"RoundedRectY", valpoint))
	//		{
	//			valpoint.ConvertToFloat(yRound);
	//		}
	//		roundedRect.rect = area;
	//		roundedRect.radiusX = xRound;
	//		roundedRect.radiusY = yRound;
	//	}
	//	else if (valpoint.Compare(L"Custom"))
	//	{

	//	}
	//}

	if (attributes.retrieveEntry(L"id", valpoint) || attributes.retrieveEntry(L"ID", valpoint))
	{
		ID.Set(valpoint);
	}

	if (attributes.retrieveEntry(L"Class", valpoint))
	{
		TrecPointer<TDataArray<TString>> classes = className.split(L";");

		TDataMap<TString> styleList;

		for (int c = 0; c < classes->Size(); c++)
		{
			TString style(classes->at(c));
			if (style.GetSize() && styles.retrieveEntry(style, styleList))
			{
				OnCreateStyle(styleList, d);
			}
		}
	}


	OnCreateStyle(attributes, d);
	this->SetUpTextElement();
	if (border.Get())
		border->onCreate(area);
	if (content.Get())
		content->onCreate(area);

	if (attributes.retrieveEntry(TString(L"FixedHeight"), valpoint))
	{
		if (!valpoint.Compare(L"true"))
			fixedHeight = true;
	}

	if (attributes.retrieveEntry(TString(L"FixedWidth"), valpoint))
	{
		if (!valpoint.Compare(L"true"))
			fixedWidth = true;
	}

	return false;
}

void TControl::updateArrayID(int aid)
{
	TObjectLocker threadLock(&thread);
	arrayID = aid;

}

RECT_F TControl::getMargin()
{
	TObjectLocker threadLock(&thread);
	auto ret = margin;

	return ret;
}

bool TControl::SetMargin(const RECT_F& newMargin)
{
	if ((bounds.left + newMargin.left) > (bounds.right - newMargin.right))
		return false;

	if ((bounds.top + newMargin.top) > (bounds.right - newMargin.right))
		return false;

	margin = newMargin;
	return true;
}

void TControl::ShiftHorizontal(int degrees)
{
	TObjectLocker threadLock(&thread);
	area.left += degrees;
	area.right += degrees;

	if (border.Get())
		border->ShiftHorizontal(degrees);
	if (content.Get())
		content->ShiftHorizontal(degrees);
}

void TControl::ShiftVertical(int degrees)
{
	TObjectLocker threadLock(&thread);
	area.top += degrees;
	area.bottom += degrees;

	if (border.Get())
		border->ShiftVertical(degrees);
	if (content.Get())
		content->ShiftVertical(degrees);

}


bool TControl::SetDimension(UINT dimensionValue, dimension_spec spec)
{
	TObjectLocker threadLock(&thread);
	if (!dimensions.Get())
		dimensions = TrecPointerKey::GetNewTrecPointer<Dimensions>();

	switch (spec)
	{
	case dimension_spec::ds_height:
		if ((dimensions->minHeight && dimensionValue < dimensions->minHeight) ||
			(dimensions->maxHeight && dimensionValue > dimensions->maxHeight))
			return false;
		dimensions->height = dimensionValue;
		break;
	case dimension_spec::ds_height_max:
		if ((dimensions->height && dimensionValue < dimensions->height) ||
			(dimensions->minHeight && dimensionValue < dimensions->minHeight))
			return false;
		dimensions->maxHeight = dimensionValue;
		break;
	case dimension_spec::ds_height_min:
		if ((dimensions->height && dimensionValue > dimensions->height) ||
			(dimensions->maxHeight && dimensionValue > dimensions->maxHeight))
			return false;
		dimensions->minHeight = dimensionValue;
		break;


	case dimension_spec::ds_width:
		if ((dimensions->minWidth && dimensionValue < dimensions->minWidth) ||
			(dimensions->maxWidth && dimensionValue > dimensions->maxWidth))
			return false;
		dimensions->width = dimensionValue;
		break;
	case dimension_spec::ds_width_max:
		if ((dimensions->width && dimensionValue < dimensions->width) ||
			(dimensions->minWidth && dimensionValue < dimensions->minWidth))
			return false;
		dimensions->maxWidth = dimensionValue;
		break;
	case dimension_spec::ds_width_min:
		if ((dimensions->width && dimensionValue > dimensions->width) ||
			(dimensions->maxWidth && dimensionValue > dimensions->maxWidth))
			return false;
		dimensions->minWidth = dimensionValue;
	}

	return true;
}

UINT TControl::GetDimension(dimension_spec spec)
{
	if (!dimensions.Get())
		return 0;

	switch (spec)
	{
	case dimension_spec::ds_height:
		return dimensions->height;
	case dimension_spec::ds_height_max:
		return dimensions->maxHeight;
	case dimension_spec::ds_height_min:
		return dimensions->minHeight;


	case dimension_spec::ds_width:
		return dimensions->width;
	case dimension_spec::ds_width_max:
		return dimensions->maxWidth;
	case dimension_spec::ds_width_min:
		return dimensions->minWidth;
	}
}
void TControl::AddClass(const TString& t)
{
	TObjectLocker threadLock(&thread);
	TString trimmedT = t.GetTrim();
	if (className.GetSize())
	{

		className += L";";
		className += trimmedT;
	}
	else
		className = trimmedT;

}

TString TControl::GetID()
{
	TObjectLocker threadLock(&thread);
	TString ret(ID);

	return ret;
}

bool TControl::HandlesEvents()
{
	return false;
}

void TControl::Draw(TrecPointer<TVariable> object)
{
	RECT_F useArea = { 0,0,0,0 };
	if (!isActive || !TColorBrush::NormalizeRect(useArea, area, this->drawingBoard))
		return;
	if (content.Get())
		content->onDraw(useArea);
	if (text.Get())
		text->OnDraw(object);
	if (border.Get())
		border->onDraw(useArea);
}

void TControl::OnRButtonUp(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& pages)
{
	if (!isActive)
		return;
	if (DrawingBoard::IsContained(point, area))
	{
		mOut = this->overrideParent ? message_output::mo_positive_override : message_output::mo_positive_continue;

		if (isRightClicked)
		{
			TString index = HasEvent(R_Message_Type::On_Right_Click);
			if (index.GetSize())
			{
				this->args.Reset();
				this->args.arrayLabel = this->arrayID;
				this->args.eventType = R_Message_Type::On_Right_Click;
				this->args.isClick = true;
				this->args.isLeftClick = false;
				this->args.methodID = index;
				this->args.point = point;
				this->args.positive = true;
				this->args.type = L'\0';
				EventID_Cred cred(R_Message_Type::On_Right_Click, TrecPointerKey::TrecFromSoft<>(self));
				cred.args = TrecPointerKey::GetNewTrecPointer<EventArgs>(this->args);

				pages.push_back(cred);
			}
		}
		else
		{
			TString index = HasEvent(R_Message_Type::On_R_Button_Up);
			if (index.GetSize())
			{
				this->args.Reset();
				this->args.arrayLabel = this->arrayID;
				this->args.eventType = R_Message_Type::On_R_Button_Up;
				this->args.isClick = true;
				this->args.isLeftClick = false;
				this->args.methodID = index;
				this->args.point = point;
				this->args.positive = true;
				this->args.type = L'\0';
				EventID_Cred cred(R_Message_Type::On_R_Button_Up, TrecPointerKey::TrecFromSoft<>(self));
				cred.args = TrecPointerKey::GetNewTrecPointer<EventArgs>(this->args);

				pages.push_back(cred);
			}
		}
	}
	isRightClicked = false;
}

void TControl::OnRButtonDown(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& pages)
{
	if (!isActive)
		return;
	if (DrawingBoard::IsContained(point, area))
	{
		mOut = this->overrideParent ? message_output::mo_positive_override : message_output::mo_positive_continue;

		TString index = HasEvent(R_Message_Type::On_R_Button_Down);
		if (index.GetSize())
		{
			this->args.Reset();
			this->args.arrayLabel = this->arrayID;
			this->args.eventType = R_Message_Type::On_R_Button_Down;
			this->args.isClick = true;
			this->args.isLeftClick = false;
			this->args.methodID = index;
			this->args.point = point;
			this->args.positive = true;
			this->args.type = L'\0';
			EventID_Cred cred(R_Message_Type::On_R_Button_Down, TrecPointerKey::TrecFromSoft<>(self));
			cred.args = TrecPointerKey::GetNewTrecPointer<EventArgs>(this->args);

			pages.push_back(cred);
		}

		isRightClicked = true;
	}
}

void TControl::OnLButtonUp(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& eventAr)
{
	if (!isActive)
		return;
	if (DrawingBoard::IsContained(point, area))
	{
		mOut = this->overrideParent ? message_output::mo_positive_override : message_output::mo_positive_continue;

		if (isLeftClicked)
		{
			TString index = HasEvent(R_Message_Type::On_Click);

			//EventID_Cred cred(R_Message_Type::On_Click, TrecPointerKey::TrecFromSoft<>(self), text.Get() ? text->GetTextInterceptor() : TrecPointer<TTextIntercepter>());

			//if (index.GetSize())
			//{
			//	this->args.Reset();
			//	this->args.arrayLabel = this->arrayID;
			//	this->args.eventType = R_Message_Type::On_Click;
			//	this->args.isClick = true;
			//	this->args.isLeftClick = true;
			//	this->args.methodID = index;
			//	this->args.point = point;
			//	this->args.positive = true;
			//	this->args.type = L'\0';

			//	cred.args = TrecPointerKey::GetNewTrecPointer<EventArgs>(this->args);
			//}
			//if (text.Get() || index.GetSize())
			//	eventAr.push_back(cred);

		}
		else
		{
			TString index = HasEvent(R_Message_Type::On_L_Button_Up);
			if (index.GetSize())
			{
				this->args.Reset();
				this->args.arrayLabel = this->arrayID;
				this->args.eventType = R_Message_Type::On_L_Button_Up;
				this->args.isClick = true;
				this->args.isLeftClick = true;
				this->args.methodID = index;
				this->args.point = point;
				this->args.positive = true;
				this->args.type = L'\0';
				EventID_Cred cred(R_Message_Type::On_L_Button_Up, TrecPointerKey::TrecFromSoft<>(self));
				cred.args = TrecPointerKey::GetNewTrecPointer<EventArgs>(this->args);

				eventAr.push_back(cred);
			}
		}
	}
	isLeftClicked = false;
}

void TControl::OnMouseMove(UINT nFlags, TPoint point, message_output& mOut, TDataArray<EventID_Cred>& pages)
{
	if (!isActive)
		return;
	if (DrawingBoard::IsContained(point, area))
	{
		mOut = this->overrideParent ? message_output::mo_positive_override : message_output::mo_positive_continue;
		if (!isMouseIn)
		{
			TString index = HasEvent(R_Message_Type::On_Hover_Enter);
			if (index.GetSize())
			{
				this->args.Reset();
				this->args.arrayLabel = this->arrayID;
				this->args.eventType = R_Message_Type::On_Hover_Enter;
				this->args.isClick = false;
				this->args.isLeftClick = false;
				this->args.methodID = index;
				this->args.point = point;
				this->args.positive = true;
				this->args.type = L'\0';
				EventID_Cred cred(R_Message_Type::On_Hover_Enter, TrecPointerKey::TrecFromSoft<>(self));
				cred.args = TrecPointerKey::GetNewTrecPointer<EventArgs>(this->args);

				pages.push_back(cred);
			}
		}
		isMouseIn = true;
		TString index = HasEvent(R_Message_Type::On_Hover);
		if (index.GetSize())
		{
			this->args.Reset();
			this->args.arrayLabel = this->arrayID;
			this->args.eventType = R_Message_Type::On_Hover;
			this->args.isClick = false;
			this->args.isLeftClick = false;
			this->args.methodID = index;
			this->args.point = point;
			this->args.positive = true;
			this->args.type = L'\0';
			EventID_Cred cred(R_Message_Type::On_Hover, TrecPointerKey::TrecFromSoft<>(self));
			cred.args = TrecPointerKey::GetNewTrecPointer<EventArgs>(this->args);

			pages.push_back(cred);
		}

	}
	else
	{
		if (isMouseIn)
		{
			TString index = HasEvent(R_Message_Type::On_Hover_Leave);
			if (index.GetSize())
			{
				this->args.Reset();
				this->args.arrayLabel = this->arrayID;
				this->args.eventType = R_Message_Type::On_Hover_Leave;
				this->args.isClick = true;
				this->args.isLeftClick = true;
				this->args.methodID = index;
				this->args.point = point;
				this->args.positive = true;
				this->args.type = L'\0';
				EventID_Cred cred(R_Message_Type::On_Hover_Leave, TrecPointerKey::TrecFromSoft<>(self));
				cred.args = TrecPointerKey::GetNewTrecPointer<EventArgs>(this->args);

				pages.push_back(cred);
			}
		}
		isMouseIn = false;
	}
}

void TControl::OnLButtonDblClk(UINT nFlags, TPoint point, message_output& mOut, TDataArray<EventID_Cred>& args)
{
	if (!isActive)
		return;
	if (DrawingBoard::IsContained(point, area))
	{
		mOut = this->overrideParent ? message_output::mo_positive_override : message_output::mo_positive_continue;

		TString index = HasEvent(R_Message_Type::On_LDoubleClick);
		if (index.GetSize())
		{
			this->args.Reset();
			this->args.arrayLabel = this->arrayID;
			this->args.eventType = R_Message_Type::On_LDoubleClick;
			this->args.isClick = true;
			this->args.isLeftClick = true;
			this->args.methodID = index;
			this->args.point = point;
			this->args.positive = true;
			this->args.type = L'\0';
			EventID_Cred cred(R_Message_Type::On_LDoubleClick, TrecPointerKey::TrecFromSoft<>(self));
			cred.args = TrecPointerKey::GetNewTrecPointer<EventArgs>(this->args);

			args.push_back(cred);
		}

	}
}

void TControl::OnLButtonDown(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& pages)
{
	if (!isActive)
		return;
	if (DrawingBoard::IsContained(point, area))
	{
		mOut = this->overrideParent ? message_output::mo_positive_override : message_output::mo_positive_continue;

		TString index = HasEvent(R_Message_Type::On_L_Button_Down);
		if (index.GetSize())
		{
			this->args.Reset();
			this->args.arrayLabel = this->arrayID;
			this->args.eventType = R_Message_Type::On_L_Button_Down;
			this->args.isClick = true;
			this->args.isLeftClick = true;
			this->args.methodID = index;
			this->args.point = point;
			this->args.positive = true;
			this->args.type = L'\0';
			EventID_Cred cred(R_Message_Type::On_L_Button_Down, TrecPointerKey::TrecFromSoft<>(self));
			cred.args = TrecPointerKey::GetNewTrecPointer<EventArgs>(this->args);
			pages.push_back(cred);;
		}


		isLeftClicked = true;
	}
}

void TControl::OnResize(RECT_F& newLoc, UINT nFlags, TDataArray<EventID_Cred>& eventAr)
{
	bounds = newLoc;
	auto curLoc = area;
	SetSize();

	TString index = HasEvent(R_Message_Type::On_Resized);
	if (index.GetSize())
	{
		this->args.Reset();
		this->args.arrayLabel = this->arrayID;
		this->args.eventType = R_Message_Type::On_Resized;
		this->args.isClick = false;
		this->args.isLeftClick = false;
		this->args.methodID = index;
		this->args.point.x = 0;
		this->args.point.y = 0;
		this->args.positive = true;
		this->args.type = L'\0';
		this->args.oldSize = curLoc;
		this->args.newSize = area;

		EventID_Cred cred(R_Message_Type::On_Resized, TrecPointerKey::TrecFromSoft<>(self));
		cred.args = TrecPointerKey::GetNewTrecPointer<EventArgs>(this->args);

		eventAr.push_back(cred);
	}
	if (border.Get())
		border->loci = area;
	if (content.Get())
		content->loci = area;
	if (text.Get())
		text->SetBounds(area);
}

bool TControl::OnDestroy()
{
	return true;
}

bool TControl::OnScroll(bool, const TPoint& point, const TPoint& direction, TDataArray<EventID_Cred>& args)
{
	area.bottom += direction.y;
	area.top += direction.y;
	area.left += direction.x;
	area.right += direction.x;

	if (border.Get())
		border->loci = area;
	if (content.Get())
		content->loci = area;
	if (text.Get())
		text->SetBounds(area);

	if (point.x > 0 && point.y > 0)
	{
		TString index = HasEvent(R_Message_Type::On_Scrolled);
		if (index.GetSize())
		{
			this->args.Reset();
			this->args.arrayLabel = this->arrayID;
			this->args.eventType = R_Message_Type::On_Scrolled;
			this->args.isClick = false;
			this->args.isLeftClick = false;
			this->args.methodID = index;
			this->args.point = point;
			this->args.positive = true;
			this->args.type = L'\0';
			EventID_Cred cred(R_Message_Type::On_Scrolled, TrecPointerKey::TrecFromSoft<>(self));
			cred.args = TrecPointerKey::GetNewTrecPointer<EventArgs>(this->args);

			args.push_back(cred);
		}
		return true;
	}

	return false;
}

float TControl::GetMinWidth()
{
	float limitMin = dimensions.Get() ? dimensions->minWidth : 1.0f;
	float textWidth(0.0f);
	if (text.Get())
		text->GetMinWidth(textWidth, 0);
	return std::max(limitMin, textWidth);
}

void TControl::ShrinkHeight()
{
	TObjectLocker threadLock(&thread);
	float bottom = area.top;

	if (text.Get())
	{
		float tWidth(0.0f);

		

		bool w = text->GetMinHeight(tWidth);

		assert(w);
		bottom += tWidth;
	}

	if (bottom > area.top)
		area.bottom = bottom;

}

bool TControl::InjectChildTemplate(TrecPointer<TPage> page)
{
	// Regular TControls do not take in Children
	return false;
}

bool TControl::SupportsChildTemplateInjection()
{
	return false;
}

void TControl::InspectEventAttributes()
{
	TString valpoint;
	for (UINT Rust = 0; Rust < ARRAYSIZE(eventsInStringForm); Rust++)
	{
		if (attributes.retrieveEntry(eventsInStringForm[Rust].str, valpoint))
			eventList.push_back(EventTypeID(eventsInStringForm[Rust].type, valpoint));
	}
}

TString TControl::HasEvent(R_Message_Type mType)
{
	for (UINT Rust = 0; Rust < eventList.Size(); Rust++)
	{
		if (eventList[Rust].eventType == mType)
			return eventList[Rust].eventID;
	}
	return L"";
}

void TControl::OnCreateSize()
{
	area = bounds;

	TString valpoint;
	UINT dime = 0;
	if (attributes.retrieveEntry(L"Height", valpoint) && TString::ConvertStringToUint(valpoint, dime))
	{
		SetDimension(dime, dimension_spec::ds_height);
	}
	if (attributes.retrieveEntry(L"MaxHeight", valpoint) && TString::ConvertStringToUint(valpoint, dime))
	{
		SetDimension(dime, dimension_spec::ds_height_max);
	}
	if (attributes.retrieveEntry(L"MinHeight", valpoint) && TString::ConvertStringToUint(valpoint, dime))
	{
		SetDimension(dime, dimension_spec::ds_height_min);
	}


	if (attributes.retrieveEntry(L"Width", valpoint) && TString::ConvertStringToUint(valpoint, dime))
	{
		SetDimension(dime, dimension_spec::ds_width);
	}
	if (attributes.retrieveEntry(L"MaxWidth", valpoint) && TString::ConvertStringToUint(valpoint, dime))
	{
		SetDimension(dime, dimension_spec::ds_width_max);
	}
	if (attributes.retrieveEntry(L"MinWidth", valpoint) && TString::ConvertStringToUint(valpoint, dime))
	{
		SetDimension(dime, dimension_spec::ds_width_min);
	}


	if (attributes.retrieveEntry(L"Margin", valpoint))
		SetMargin(ConvertStringToD2D1Rect(valpoint));

	SetSize();
}

void TControl::SetSize()
{
	area.top = bounds.top + margin.top;
	area.left = bounds.left + margin.left;
	area.right = bounds.right - margin.right;
	area.bottom = bounds.bottom - margin.bottom;


	float curWidth = area.right - area.left;
	float curHeight = area.bottom - area.top;

	UINT maxDime = GetDimension(dimension_spec::ds_width_max),
		minDime = GetDimension(dimension_spec::ds_width_min);

	if (maxDime && curWidth > maxDime)
	{
		area.right = area.left + maxDime;
	}
	else if (minDime && curWidth < minDime)
	{
		RECT_F newBounds = bounds;
		UINT diff = curWidth - minDime;
		area.right += diff;
		newBounds.right += diff;


		TrecPointer<TPage> tParent = TrecPointerKey::TrecFromSoft<>(parent);
		if (tParent.Get())
		{
			tParent->InjectScrollerPage(bounds, newBounds, TrecPointerKey::TrecFromSoft<>(self));
			bounds = newBounds;
		}
	}


	maxDime = GetDimension(dimension_spec::ds_height_max);
	minDime = GetDimension(dimension_spec::ds_height_min);

	if (maxDime && curHeight > maxDime)
	{
		area.bottom = area.top + maxDime;
	}
	else if (minDime && curHeight < minDime)
	{
		RECT_F newBounds = bounds;
		UINT diff = curHeight - minDime;
		area.bottom += diff;
		newBounds.bottom += diff;

		TrecPointer<TPage> tParent = TrecPointerKey::TrecFromSoft<>(parent);
		if (tParent.Get())
		{
			tParent->InjectScrollerPage(bounds, newBounds, TrecPointerKey::TrecFromSoft<>(self));
			bounds = newBounds;
		}
	}
}


void TControl::SetUpTextElement()
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
		fontColor.SetColor(TColor());

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
	if (actText.GetSize())
	{
		details.defaultTextColor.SetColor(fontColor);

		text = TrecPointerKey::GetNewTrecPointer<TTextElement>(drawingBoard);

		text->SetBounds(area);
		text->SetBasicFormatting(details);
		text->SetHorizontalAllignment(hAlign);
		text->SetVerticalAllignment(vAlign);
		text->SetText(actText);
	}
}

void TControl::OnCreateStyle(TDataMap<TString>& atts, TrecPointer<TFileShell> d)
{
	TString valpoint;

	if (atts.retrieveEntry(L"BorderThickness", valpoint))
	{
		if (!border.Get())
			border = TControlComponent::GetControlBorder(drawingBoard, TrecPointerKey::TrecFromSoft<>(self));  //TrecPointerKey::GetNewTrecPointer<TControlComponent>(drawingBoard, (this));
		valpoint.ConvertToFloat((border->thickness));         // Logic Bug needs fixing
	}

	bool worked;
	if (atts.retrieveEntry(L"BorderColor", valpoint))
	{
		if (!border.Get())
			border = TControlComponent::GetControlBorder(drawingBoard, TrecPointerKey::TrecFromSoft<>(self));

		TColor::GetColorFromString(valpoint, worked);
		border->stopCollection.AddGradient(TGradientStop(TColor::GetColorFromString(valpoint, worked), 0.0f));
	}


	if (atts.retrieveEntry(L"ContentThickness", valpoint))
	{
		if (!content.Get())
			content = TControlComponent::GetControlContent(drawingBoard, TrecPointerKey::TrecFromSoft<>(self));
		valpoint.ConvertToFloat((content->thickness));
	}

	if (atts.retrieveEntry(L"ContentColor", valpoint))
	{
		if (!content.Get())
			content = TControlComponent::GetControlContent(drawingBoard, TrecPointerKey::TrecFromSoft<>(self));
		content->stopCollection.AddGradient(TGradientStop(TColor::GetColorFromString(valpoint, worked), 0.0f));
	}

	if (atts.retrieveEntry(L"ContentGrad", valpoint))
	{
		if (!content.Get())
			content = TControlComponent::GetControlContent(drawingBoard, TrecPointerKey::TrecFromSoft<>(self));
		float entry = 0.0f;
		valpoint.ConvertToFloat(entry);
		//content->secondColor = true;
		UINT gradCount = 0;
		if (gradCount = content->stopCollection.GetGradientCount())
			content->stopCollection.SetPositionAt(entry, gradCount - 1);
	}

	if (atts.retrieveEntry(L"BorderGrad", valpoint))
	{
		if (!border.Get())
			border = TControlComponent::GetControlBorder(drawingBoard, TrecPointerKey::TrecFromSoft<>(self));
		float entry = 0.0f;
		valpoint.ConvertToFloat(entry);
		//border->secondColor = true;
		UINT gradCount = 0;
		if (gradCount = border->stopCollection.GetGradientCount())
			border->stopCollection.SetPositionAt(entry, gradCount - 1);
	}

	if (atts.retrieveEntry(L"ContentGradMode", valpoint))
	{
		if (!content.Get())
			content = TControlComponent::GetControlContent(drawingBoard, TrecPointerKey::TrecFromSoft<>(self));
		if (valpoint.Compare(L"Radial"))
			content->useRadial = true;
	}
	if (atts.retrieveEntry(L"BorderGradMode", valpoint))
	{
		if (!border.Get())
			border = TControlComponent::GetControlBorder(drawingBoard, TrecPointerKey::TrecFromSoft<>(self));
		if (valpoint.Compare(L"Radial"))
			border->useRadial = true;
	}

	if (atts.retrieveEntry(L"ImageSource", valpoint))
	{
		if (!content.Get() || !d.Get())
			content = TControlComponent::GetControlContent(drawingBoard, TrecPointerKey::TrecFromSoft<>(self));
		valpoint.Replace(L"/", L"\\");

		TrecPointer<TFileShell> file = TFileShell::GetFileInfo(d->GetPath() + L"\\" + valpoint);

		if (file.Get())
		{
			TObjectLocker threadLock(&thread);
			content->image = TrecPointerKey::ConvertPointer<TBrush, TImageBrush>(drawingBoard->GetImageBrush(file));
		}
	}


}

/**
 * Function: convertStringToTextAlignment
 * Purpose: Reads the String and returns whether the Text should be right, left, center, or justified
 * Parameters: TString* t - the string to parse
 * Returns: DWRITE_TEXT_ALIGNMENT - the Text position (Center if string is invalid)
 */
tc_line_spacing convertStringToTextAlignment(const TString& t)
{

	if (!t.CompareNoCase(L"left"))
		return tc_line_spacing::left;
	if (!t.CompareNoCase(L"right"))
		return tc_line_spacing::right;
	if (!t.CompareNoCase(L"justified"))
		return tc_line_spacing::justified;
	return tc_line_spacing::center;
}


/**
 * Function: convertStringToParagraphAlignment
 * Purpose: Reads the String and returns whether the Text should be top, bottom, or center
 * Parameters: TString* t - the string to parse
 * Returns: DWRITE_PARAGRAPH_ALIGNMENT - the text alignment (Center if string is invalid)
 */
tc_text_spacing convertStringToParagraphAlignment(const TString& t)
{
	if (!t.CompareNoCase(L"top"))
		return tc_text_spacing::top;
	if (!t.CompareNoCase(L"bottom"))
		return tc_text_spacing::bottom;
	return tc_text_spacing::center;
}


/*
 * Function: isContained
 * Purpose: Checks of a point is within a given Direct2D Rectangle
 * Parameters: const TPoint& - the point to check
 *				const RECT_F& - the rectangle to check
 * Returns: bool - whether the point is withing the bounds
 */
bool IsContained(const TPoint& cp, const ELLIPSE_F& el)
{
	return (pow((cp.x - el.point.x), 2) / pow(el.x, 2)
		+ pow((cp.y - el.point.y), 2) / pow(el.y, 2)) <= 1;
}