#pragma once
#include <TString.h>
#include "AnaWidget.h"
#include <TPoint.h>
#include <TObjectNode.h>
#include <GraphicsDef.h>
#include <DrawingBoard.h>

#define ag_msg

using R_Message_Type = enum class R_Message_Type
{
	On_L_Button_Down,  // User presses down on left mouse button
	On_L_Button_Up,    // User releases left mouse button
	On_R_Button_Down,  // User presses down on right mouse button
	On_R_Button_Up,    // User releases right mouse button
	On_Click,          // User presses down and releases left mouse button
	On_Hold_Click,     // (Not implemented)
	On_Hover,          // Mouse is over the control
	On_Hover_Enter,		// Mouse just enetered the control's space
	On_Hover_Leave,    // Mouse leaves the control
	On_Right_Click,    // User presses down and releases right mouse button
	On_Click_Release,  // (deprecated)
	On_Text_Change,    // The text of this control has changed
	On_Right_Release,  // (deprecated)
	On_sel_change,     // A Selection has changed
	On_check,          // a Checkbox status has been changed
	On_radio_change,   // A Radio Button group has been updated
	On_Char,           // A Character has been entered
	On_Focus,          // A Control is now the focus
	On_Lose_Focus,     // A Control no longer has focus
	On_Select_Scroller,// A Scroll bar is involved
	On_Flyout,         // A control is to appear in a flyout
	On_LDoubleClick,    // Double Click detected
	On_Scrolled,		// Control was scrolled
	On_Resized,
	On_SubmitDrag		// Added to Event-Cred when a Page is being dragged around
};

class _ANA_WIDGET EventArgs : public TObject
{
public:
	EventArgs();
	void Reset();
	EventArgs(const EventArgs& args);

	TString text;
	bool positive;
	TPoint point;
	bool isClick;
	bool isLeftClick;
	R_Message_Type eventType;
	// TrecPointer<TPage> control;
	TString methodID;
	int arrayLabel;
	WCHAR type;
	TrecPointer<TObjectNode> object;
	RECT_F newSize, oldSize;
};

class _ANA_WIDGET Dimensions: public TObject
{
public:
	Dimensions();
	UINT width;
	UINT height;
	UINT minHeight;
	UINT minWidth;
	UINT maxWidth;
	UINT maxHeight;
};

typedef enum class message_output
{
	mo_negative, // message has not been intercepted
	mo_positive_continue, // message has been intercepted by a child page but parent page is free to add its own events
	mo_positive_override, // message has been intercepted by a child page and parent should take no further action
	mo_positive_handled, // message has been intercepted by a child page and has handled events generated
	mo_positive_scroll   // Message was intercepted by a Scroll Bar
}message_output;

typedef enum class ScrollOrient
{
	so_vertical, // ScrollBar is Vertical (most common)
	so_horizontal // Scrollbar is Horizontal (less common but still possible)
}ScrollOrient;

class _ANA_WIDGET TPage :
    public TVObject
{
public:
	// Classes that support the TPage

	class _ANA_WIDGET TScrollBar : public TObject
	{
	public:

		TScrollBar(TrecPointer<TPage> control, ScrollOrient so);
		~TScrollBar();

		void onDraw(TrecPointer<DrawingBoard> target);

		virtual bool OnLButtonDown(UINT nFlags, TPoint point, message_output& mOut);
		virtual void OnLButtonUp(UINT nFlags, TPoint point, message_output& mOut);
		virtual void OnMouseMove(UINT nFlags, TPoint point, message_output& mOut);

		float MovedContent(float degree);

		void Refresh(const RECT_F& location, const RECT_F& area);

	private: // Static attributes, styles that should be shared across all Scroll Bars

		/**
		 * Two imporant rectangles to draw
		 */
		RECT_F body_rect, scroll_rect;

		/**
		 * Parent holding this Scroll Bar
		 */
		TrecPointerSoft<TPage> parent;
		/**
		 * Used to track movement of the mouse to determine how much to shift by
		 */
		TPoint prevPoint;
		/**
		 * Whether the bar is being focused on (drawable currently not used)
		 */
		bool onFocus, drawable;
		/**
		 * Distance between the regions and the ratio provided
		 */
		float diff1, diff2, widthFactor;
		/**
		 * Vertical or horizontal
		 */
		ScrollOrient scrollAlignment;
	};

	class _ANA_WIDGET EventID_Cred
	{
	public:
		EventID_Cred();
		EventID_Cred(const EventID_Cred& copy);
		EventID_Cred(R_Message_Type t, TrecPointer<TPage> c);
		EventID_Cred(R_Message_Type t, TrecPointer<TPage> c, TrecPointer<TScrollBar> sb);
		EventID_Cred(R_Message_Type t, TrecPointer<TPage> c, TrecPointer<TTextIntercepter> i);


		R_Message_Type eventType;
		TrecPointer<TPage> control;
		TrecPointer<TScrollBar> scroll;
		// TrecPointer<TFlyout> flyout; // To-Do: Re add later
		TrecPointer<TTextIntercepter> textIntercepter;
		TrecPointer<TVariable> data;		// Holds either a variable, or an interpretor to handle an event
		TString expression;

		TrecPointer<EventArgs> args;
	};

	class _ANA_WIDGET EventHandler : public TObject
	{
	public:

		/**
		 * Method: EventHandler::Initialize
		 * Purpose: Initializes the Handler so that it has direct Access to certain Controls held by the page
		 * Parameters: TrecPointer<Page> page - page that holds the Controls to latch on to
		 * Returns: void
		 *
		 * Attributes: abstract
		 */
		virtual void Initialize(TrecPointer<TPage> page) = 0;

		/**
		 * Method: EventHandler::OnChar
		 * Purpose: Handles Character input from the Instance of Anagame
		 * Parameters: bool fromChar - true if it came form On_Char from windows, false if it came from a key_down
		 *				UINT nChar - the character submitted
		 *				UINT nRepCnt - number od times to add the character
		 *				UINT nFlags - unused
		 *				message_output* mOut - result
		 * Returns: bool
		 */
		virtual bool OnChar(bool fromChar, UINT nChar, UINT nRepCnt, UINT nFlags, message_output* mOut) = 0;

		virtual void SetSelf(TrecPointer<TPage::EventHandler> handleSelf) = 0;

		virtual TrecPointer<TPage> GetPage() = 0;

		virtual void HandleEvents(TDataArray<TPage::EventID_Cred>& eventAr) = 0;

		//virtual TrecPointer<TTextIntercepter> GetTextIntercepter() = 0;
	};

	public:

		ObjectType GetObjectType()override;

		virtual bool GetVariable(const TString& name, TrecPointer<TVariable>& var) override;

		virtual bool SetVariable(const TString& name, TrecPointer<TVariable> var) override;

		virtual bool IsScroller();

		TPage(TrecPointer<DrawingBoard> board);

		virtual bool HandlesEvents() = 0;

		virtual TString PrepPage(TrecPointer<TFileShell> file, TrecPointer<EventHandler> handler);

		virtual void Draw(TrecPointer<TVariable> object) = 0;

		ag_msg virtual void OnRButtonUp(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>&) = 0;
		ag_msg virtual void OnRButtonDown(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>&) = 0;
		ag_msg virtual void OnLButtonUp(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>&) = 0;
		ag_msg virtual void OnLButtonDown(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>&) = 0;
		ag_msg virtual void OnMouseMove(UINT nFlags, TPoint point, message_output& mOut, TDataArray<EventID_Cred>&) = 0;
		ag_msg virtual void OnLButtonDblClk(UINT nFlags, TPoint point, message_output& mOut, TDataArray<EventID_Cred>& eventAr) = 0;
		ag_msg virtual void OnResize(RECT_F& newLoc, UINT nFlags, TDataArray<EventID_Cred>& eventAr) = 0;
		ag_msg virtual bool OnDestroy() = 0;
		ag_msg virtual bool OnScroll(bool fromBars, const TPoint& point, const TPoint& direction, TDataArray<EventID_Cred>&) = 0;

		virtual void SetSelf(TrecPointer<TPage>);

		TrecPointer<TPage> GetParentPage();

		void SetParentPage(TrecPointer<TPage> parent);

		virtual RECT_F GetArea();

		void RotateDegrees(float degrees);
		void RotateRadians(float radians);

		virtual void InjectScrollerPage(const RECT_F& bounds, const RECT_F& needs, TrecPointer<TPage> page);
		virtual TrecPointer<EventHandler> GetHandler();

protected:


	/**
	 * The Drawing Board to Draw against
	 */
	TrecPointer<DrawingBoard> drawingBoard;

	/**
	 * The Area to which this Page is assigned
	 */
	RECT_F area;

	/**
	 * Self-Reference and Parent
	 */
	TrecPointerSoft<TPage> self, parent;

	/**
	 * How much to rotate by before drawing
	 */
	float rotate;
};

class _ANA_WIDGET TPageBuilder :
	public TObject
{
protected:
	TrecPointer<DrawingBoard> board;
	RECT_F space;
	TrecPointer<TPage::EventHandler> handler;
public:
	ObjectType GetObjectType() override;
	virtual bool UsesFile();
	virtual TrecPointer<TPage> GetPage(const TString& details) = 0;
	virtual TrecPointer<TPage> GetPage(TrecPointer<TFileShell> file) = 0;

	void SetDrawingBoard(TrecPointer<DrawingBoard> board);
	void SetSpace(RECT_F space);
	void SetHandler(TrecPointer<TPage::EventHandler> handler);
};

RECT_F ConvertStringToD2D1Rect(const TString& str);

UINT GetScrollbarBoxSize();