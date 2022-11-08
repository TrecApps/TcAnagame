#pragma once
#include "TPage.h"
#include <TImageBrush.h>
#include <TDataMap.h>
#include <TTextElement.h>
#include <TColorBrush.h>
#include <TGradientStopCollection.h>

/**
 * Enum Class: dimension_spec
 * Purpose: Indicated the Dimension Detail to work with
 */
typedef enum class dimension_spec
{
	ds_width,
	ds_width_min,
	ds_width_max,
	ds_height,
	ds_height_min,
	ds_height_max,
}dimension_spec;

/**
 * Enum Class: messageState
 * Purpose: Used by the control to determine which to draw
 */
typedef enum class  messageState
{
	normal,      // Control is in normal mode
	mouseHover,  // Control is in Mouse-Honer mode
	mouseLClick, // Control is in Mouse-Click mode
	mouseRClick  // Control is in Mouse-Right-Click mode
} messageState;

/**
 * Class: EventTypeId
 * Purpose: Stored by Controls for checking whether or not they actually have a message handler
 */
class _ANA_WIDGET EventTypeID
{
public:
	EventTypeID();
	EventTypeID(R_Message_Type type, const TString& id);
	EventTypeID(const EventTypeID& copy);

	R_Message_Type eventType;
	TString eventID;
};

/**
 * Enum Class: TShape
 * Purpose: Determines the basic size of the TControl
 * NOTE: Feature is unstable, stick with T_Rect for now
 */
typedef enum class TShape {
	T_Rect,
	T_Rounded_Rect,
	T_Ellipse,
	T_Custom_shape
}TShape;

/*
 * Class: TControlComponent
 * Purpose: Draws the Border of a Given TControl
 */
class _ANA_WIDGET TControlComponent : public TObject
{
	friend class TControl;
	friend class TControlComponent;
	friend class TrecPointerKey;
public:

	TControlComponent(TrecPointer<TControlComponent>&, TrecPointer<TPage> parent);
	~TControlComponent();

	// Various On Create Methods depending on the shape of the control

	bool onCreate(RECT_F);

	// Drawing Method

	void onDraw(RECT_F& loc);

	// Handling Colors
	// NOTE: For Compatibility between AnaGame for Windows and an eventual AnaGame for Unix,
	//	Parameters and Return values should be replaced with a cross-platform structure 


	// Methods for handling Opaquency. Note, there is no getOpaquency() method as it is handled by the getColored method 

	void setThickness(float f);
	void setOpaquency(float f);
	float getThickness();

	// Getting the Location

	RECT_F getLocation();
	void ShiftHorizontal(int degrees);
	void ShiftVertical(int degrees);

	TrecPointer<TColorBrush> GetBrush();
	static TrecPointer<TControlComponent> GetControlBorder(TrecPointer<DrawingBoard> board, TrecPointer<TPage> parent);

	static TrecPointer<TControlComponent> GetControlContent(TrecPointer<DrawingBoard> board, TrecPointer<TPage> parent);

	void SetImageBrush(TrecPointer<TImageBrush> newBrush);

private:

	TControlComponent(TrecPointer<DrawingBoard>, TrecPointer<TPage> parent);

	bool isContent;

	TrecPointer<TColorBrush> brush;

	void ResetBrush();

	TrecPointer <TBrush> BuilderFocusBrush;
	float thickness;
	TString style;

	TrecPointer<DrawingBoard> drawingBoard;
	TrecPointer<TImageBrush> image;

	TrecPointerSoft<TPage> parent;

	TShape shape;

	/**
	 * Holds color, used for creating the brush
	 */
	TGradientStopCollection stopCollection;

	bool useRadial;

	// Alternate shapes

	RECT_F loci;
	ELLIPSE_F circle;
	ROUNDED_RECT_F roundedRect;
};


class _ANA_WIDGET TControl :
    public TPage
{
public:
	void AddAttribute(const TString& att, const TString& value);
	TControl(TrecPointer<DrawingBoard> drawingBoard, TDataMap<TDataMap<TString>> styles);

	void setActive(bool act);

	bool GetActive();

	virtual bool onCreate(const RECT_F& loc, TrecPointer<TFileShell> d);

	void updateArrayID(int aid);

	RECT_F getMargin();

	bool SetMargin(const RECT_F& newMargin);

	void ShiftHorizontal(int degrees);

	void ShiftVertical(int degrees);

	bool SetDimension(UINT dimensionValue, dimension_spec spec);

	UINT GetDimension(dimension_spec spec);

	void AddClass(const TString& t);

	TString GetID();

	virtual bool HandlesEvents() override;

	virtual void Draw(TrecPointer<TVariable> object) override;

	ag_msg virtual void OnRButtonUp(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>&) override;
	ag_msg virtual void OnRButtonDown(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>&) override;
	ag_msg virtual void OnLButtonUp(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>&) override;
	ag_msg virtual void OnMouseMove(UINT nFlags, TPoint point, message_output& mOut, TDataArray<EventID_Cred>&) override;
	ag_msg virtual void OnLButtonDblClk(UINT nFlags, TPoint point, message_output& mOut, TDataArray<EventID_Cred>& eventAr) override;
	ag_msg virtual void OnLButtonDown(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& args) override;
	ag_msg virtual void OnResize(RECT_F& newLoc, UINT nFlags, TDataArray<EventID_Cred>& eventAr) override;
	ag_msg virtual bool OnDestroy() override;


	ag_msg virtual bool OnScroll(bool, const TPoint& point, const TPoint& direction, TDataArray<EventID_Cred>& args) override;

	virtual float GetMinWidth();

	virtual void ShrinkHeight();

	virtual bool InjectChildTemplate(TrecPointer<TPage> page);

	virtual bool SupportsChildTemplateInjection();

protected:

	virtual void SetUpTextElement();

	TDataMap<TString> attributes;

	bool fixedWidth, fixedHeight;

	TrecPointer<TControlComponent> border, content;

	TrecPointer<TTextElement> text;
	TString className;
	TString ID;

	RECT_F margin, bounds;
	ELLIPSE_F ellipse;
	ROUNDED_RECT_F roundedRect;

	TDataMap<TDataMap<TString>> styles;

	TrecPointer<TScrollBar> vScroll, hScroll;

	TShape shape;

	/**
	 * the Matrix to manage rotations and other transformations with
	 */
	//D2D1_MATRIX_3X2_F controlTransform;

	TrecPointer<Dimensions> dimensions;

	int arrayID;

	bool overrideParent;

	messageState mState;

	EventArgs args;

	/**
	 * List of Events to report given certain messages
	 */
	TDataArray<EventTypeID> eventList;

	bool isActive;

	bool isRightClicked, isLeftClicked, isMouseIn;

protected:

	void InspectEventAttributes();

	TString HasEvent(R_Message_Type mType);

	void OnCreateSize();

	void SetSize();

	void OnCreateStyle(TDataMap<TString>& atts, TrecPointer<TFileShell> d);
};

tc_line_spacing convertStringToTextAlignment(const TString& t);
tc_text_spacing convertStringToParagraphAlignment(const TString& t);