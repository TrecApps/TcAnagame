#include "TPage.h"
#include <TColorBrush.h>
#include <cstring>
#define BOX_SIZE 20
TColor end_box(0x00000066), body_box(0x00000011), middle_box_click(0x00000099), middle_box(0x00000033);

UINT GetScrollbarBoxSize()
{
	return BOX_SIZE;
}

TPage::TScrollBar::TScrollBar(TrecPointer<TPage> control, ScrollOrient so)
{
	this->parent = TrecPointerKey::SoftFromTrec<>(control);
	this->scrollAlignment = so;
}

TPage::TScrollBar::~TScrollBar() {}

void TPage::TScrollBar::onDraw(TrecPointer<DrawingBoard> target)
{
	auto actParent = TrecPointerKey::TrecFromSoft<>(parent);
	if (!target.Get() || !actParent.Get() || !(widthFactor < 1.0f)) return;

	RECT_F location = actParent->GetArea();

	auto brush = TrecPointerKey::ConvertPointer<TBrush, TColorBrush>( target->GetSolidColorBrush(body_box));
	brush->FillRectangle(body_rect);

	RECT_F d_box = body_rect;

	brush->SetColor(end_box);
	if (scrollAlignment == ScrollOrient::so_horizontal)
	{
		d_box.right = d_box.left + BOX_SIZE;

		brush->FillRectangle(d_box);

		float move = (body_rect.right - body_rect.left) - BOX_SIZE;
		d_box.right += move;
		d_box.left += move;


	}
	else
	{
		d_box.bottom = d_box.top + BOX_SIZE;

		brush->FillRectangle(d_box);

		float move = (body_rect.bottom - body_rect.top) - BOX_SIZE;
		d_box.bottom += move;
	}
	brush->FillRectangle(d_box);

	if (onFocus)
		brush->SetColor(middle_box_click);
	else
		brush->SetColor(middle_box);

	brush->FillRectangle(scroll_rect);
}


/**
 * Method: TScrollBar::OnLButtonDown
 * Purpose: Allows bars to assume focus
 * Parameters:UINT nFlags - redundant
 *				TPoint point - The point where the user clicked
 *				message_output* mOut - redundant
 * Returns: whether the focus is on this control or not
 *
 * Attributes: virtual
 */
bool TPage::TScrollBar::OnLButtonDown(UINT nFlags, TPoint point, message_output& mOut)
{
	if (DrawingBoard::IsContained(&point, body_rect))
	{
		onFocus = true;
		mOut = message_output::mo_positive_scroll;
		prevPoint = point;
	}
	else
		onFocus = false;

	return onFocus;
}

/**
 * Method: TScrollBar::OnLButtonUp
 * Purpose: Lets the scroll bar know that the user has unclicked and thus can no longer have any focus
 * Parameters: UINT nFlags - redundant
 *				TPoint point - redundant
 *				message_output* mOut - redundant
 * Returns: void
 *
 * Attributes: virtual
 */
void TPage::TScrollBar::OnLButtonUp(UINT nFlags, TPoint point, message_output& mOut)
{
	onFocus = false;
}

/**
 * Method: TScrollBar::OnMouseMove
 * Purpose: Allows the scroll bar to shift the contents along with the user (if focused upon)
 * Parameters: UINT nFlags - redundant
 *				TPoint point - current point the mouse is at
 *				message_output* mOut - redundant
 * Returns: void
 *
 * Attributes: virtual
 */
void TPage::TScrollBar::OnMouseMove(UINT nFlags, TPoint point, message_output& mOut)
{
	if (!onFocus) return;

	TDataArray<EventID_Cred> events;
	auto actParent = TrecPointerKey::TrecFromSoft<>(parent);
	

	if (scrollAlignment == ScrollOrient::so_horizontal)
	{
		if (point.x > (body_rect.right - BOX_SIZE))
			point.x = body_rect.right - BOX_SIZE;

		if (point.x < (body_rect.left + BOX_SIZE))
			point.x = body_rect.left + BOX_SIZE;

		float move = -MovedContent(point.x - prevPoint.x);
		actParent->OnScroll(true, point, TPoint(move / widthFactor, 0), events);
	}
	else
	{
		if (point.y > (body_rect.bottom - BOX_SIZE))
			point.y = body_rect.bottom - BOX_SIZE;

		if (point.y < (body_rect.top + BOX_SIZE))
			point.y = body_rect.top + BOX_SIZE;

		float move = -MovedContent(point.y - prevPoint.y);
		actParent->OnScroll(true, point, TPoint(0, move / widthFactor), events);
	}
	prevPoint = point;
}

/**
 * Method: TScrollBar::MovedContent
 * Purpose: Calculates how much to move the content by
 * Parameters: float degree - how much the user moved the mouse
 * Returns: float - how much the content itself has to move
 */
float TPage::TScrollBar::MovedContent(float degree)
{
	if (degree > 0.0f)
	{
		if (degree > diff2)
		{
			degree -= (degree - diff2);
		}
		diff2 -= degree;
		diff1 += degree;
	}
	else if (degree < 0.0f)
	{
		if (-degree > diff1)
			degree += (degree + diff1);
		diff2 -= degree;
		diff1 += degree;
	}

	return degree;
}

/**
 * Method: TScrollBar::Refresh
 * Purpose: Refreshes the scroll bars so that they are callibrated correctly
 * Parameters: const D2D1_RECT_F& location - the location of the area that is allowed to be drawn
 *				 const D2D1_RECT_F& area - the location of the region that the control[s] underneath actually take up
 * Returns: void
 */
void TPage::TScrollBar::Refresh(const RECT_F& location, const RECT_F& area)
{
	body_rect = location;
	TDataArray<EventID_Cred> args;
	auto actParent = TrecPointerKey::TrecFromSoft<>(parent);
	if (scrollAlignment == ScrollOrient::so_horizontal)
	{
		body_rect.top = body_rect.bottom - BOX_SIZE;

		diff1 = location.left - area.left;
		diff2 = area.right - location.right;

		float barSpace = body_rect.right - body_rect.left - (2 * BOX_SIZE);
		float areaSpace = area.right - area.left;
		float locationSpace = location.right - location.left;
		widthFactor = (locationSpace / areaSpace);

		scroll_rect.bottom = body_rect.bottom;
		scroll_rect.top = body_rect.top;

		scroll_rect.left = body_rect.left + BOX_SIZE + diff1 * widthFactor;
		scroll_rect.right = body_rect.right - BOX_SIZE - (diff2 * widthFactor);

		if (scroll_rect.left < (body_rect.left + BOX_SIZE))
		{
			float diff = (body_rect.left + BOX_SIZE) - scroll_rect.left;
			scroll_rect.left += diff;
			scroll_rect.right += diff;

			actParent->OnScroll(true, TPoint(-1.0, -1.0), TPoint(diff / widthFactor, 0), args);
		}
		else if (scroll_rect.right > (body_rect.right - BOX_SIZE))
		{
			float diff = (body_rect.right - BOX_SIZE) - scroll_rect.right;
			scroll_rect.left += diff;
			scroll_rect.right += diff;

			actParent->OnScroll(true, TPoint(-1.0, -1.0), TPoint(diff / widthFactor, 0), args);
		}

	}
	else
	{
		body_rect.left = body_rect.right - BOX_SIZE;

		diff1 = location.top - area.top;
		diff2 = area.bottom - location.bottom;

		float barSpace = body_rect.bottom - body_rect.top - (2 * BOX_SIZE);
		float areaSpace = area.bottom - area.top;
		float locationSpace = location.bottom - location.top;
		widthFactor = (locationSpace / areaSpace);

		scroll_rect.left = body_rect.left;
		scroll_rect.right = body_rect.right;

		scroll_rect.top = body_rect.top + BOX_SIZE + diff1 * widthFactor;
		scroll_rect.bottom = body_rect.bottom - BOX_SIZE - (diff2 * widthFactor);



		if (scroll_rect.top < (body_rect.top + BOX_SIZE))
		{
			float diff = (body_rect.top + BOX_SIZE) - scroll_rect.top;
			scroll_rect.top += diff;
			scroll_rect.bottom += diff;

			actParent->OnScroll(true, TPoint(-1.0, -1.0), TPoint(0, -(diff / widthFactor)), args);
		}
		else if (scroll_rect.bottom > (body_rect.bottom - BOX_SIZE))
		{
			float diff = (body_rect.bottom - BOX_SIZE) - scroll_rect.bottom;
			scroll_rect.top += diff;
			scroll_rect.bottom += diff;

			actParent->OnScroll(true, TPoint(-1.0, -1.0), TPoint(0, -(diff / widthFactor)), args);
		}
	}
}

TPage::EventID_Cred::EventID_Cred()
{
	this->eventType = R_Message_Type::On_Hold_Click;
}

TPage::EventID_Cred::EventID_Cred(const EventID_Cred& copy)
{
	this->control = copy.control;
	this->eventType = copy.eventType;
	this->scroll = copy.scroll;
	//this->textIntercepter = copy.textIntercepter;
	this->data = copy.data;
	this->args = copy.args;
	this->expression.Set(copy.expression);
}

TPage::EventID_Cred::EventID_Cred(R_Message_Type t, TrecPointer<TPage> c)
{
	this->eventType = t;
	this->control = c;
}

TPage::EventID_Cred::EventID_Cred(R_Message_Type t, TrecPointer<TPage> c, TrecPointer<TPage::TScrollBar> sb)
{
	this->eventType = t;
	this->control = c;
	this->scroll = sb;
}

//TPage::EventID_Cred::EventID_Cred(R_Message_Type t, TrecPointer<TPage> c, TrecPointer<TTextIntercepter> i)
//{
//	this->eventType = t;
//	this->control = c;
//	this->textIntercepter = i;
//}

void TPage::SetSelf(TrecPointer<TPage> self)
{
	if (this != self.Get())
		throw L"Not Self Reference";
	this->self = TrecPointerKey::SoftFromTrec<>(self);
}

TrecPointer<TPage> TPage::GetParentPage()
{
		return TrecPointerKey::TrecFromSoft<>(parent);
}

void TPage::SetParentPage(TrecPointer<TPage> parent)
{
	this->parent = TrecPointerKey::SoftFromTrec<>(parent);
}

RECT_F TPage::GetArea()
{
	return area;
}

void TPage::RotateDegrees(float degrees)
{
}

void TPage::RotateRadians(float radians)
{
}

void TPage::InjectScrollerPage(const RECT_F& bounds, const RECT_F& needs, TrecPointer<TPage> page)
{
	// If no children, do nothing
}

TrecPointer<TPage::EventHandler> TPage::GetHandler()
{
	return TrecPointer<TPage::EventHandler>();
}

bool TPage::GetVariable(const TString& name, TrecPointer<TVariable>& var)
{
	return false;
}

bool TPage::SetVariable(const TString& name, TrecPointer<TVariable> var)
{
	return false;
}

bool TPage::IsScroller()
{
	return false;
}

TPage::TPage(TrecPointer<DrawingBoard> board)
{
	this->drawingBoard = board;
}

TString TPage::PrepPage(TrecPointer<TFileShell> file, TrecPointer<EventHandler> handler)
{
	return L"Not Supported";
}



/**
 * Method: TScrollBar::EstablishScrollColors
 * Purpose: Establishes the colors used in the scroll bars
 * Parameters: void
 * Returns: void
 *
 * Note: Called once by the constructor
 */


EventArgs::EventArgs()
{
	Reset();
}
void EventArgs::Reset()
{
	isClick = false;
	isLeftClick = false;
	methodID = -1;
	point.x = 0.0f;
	point.y = 0.0f;
	positive = false;
	text.Empty();
	type = L'\0';
	object.Nullify();
	oldSize = newSize = { 0,0,0,0 };
}
EventArgs::EventArgs(const EventArgs& args)
{
	text.Set(args.text);
	positive = args.positive;
	point = args.point;
	isClick = args.isClick;
	isLeftClick = args.isLeftClick;
	eventType = args.eventType;
	methodID = args.methodID;
	arrayLabel = args.arrayLabel;
	type = args.type;
	object = args.object;
	oldSize = args.oldSize;
	newSize = args.newSize;
}

Dimensions::Dimensions()
{
	this->height = this->maxHeight = this->minHeight =
		this->width = this->maxWidth = this->minWidth = 0;
}


RECT_F ConvertStringToD2D1Rect(const TString& str)
{
	RECT_F returnable = RECT_F{ 0,0,0,0 };

	TrecPointer<TDataArray<TString>> strSpl = str.split(",");


	int res[4] = { 0,0,0,0 };
	if (strSpl.Get() == NULL || strSpl->Size() != 4)
	{
		return returnable;
	}
	res[0] = strSpl->at(0).ConvertToFloat(returnable.top);
	res[1] = strSpl->at(1).ConvertToFloat(returnable.left);
	res[2] = strSpl->at(2).ConvertToFloat(returnable.bottom);
	res[3] = strSpl->at(3).ConvertToFloat(returnable.right);
	if (res[0] > 0 || res[1] > 0 || res[2] > 0 || res[3] > 0)
		ZeroMemory(&returnable, sizeof(returnable));
	return returnable; LONG i;
}


