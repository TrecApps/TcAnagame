#pragma once
#include "TRandomLayout.h"

class _ANA_WIDGET LayoutSpace
{
public:
	LayoutSpace();
	LayoutSpace(const LayoutSpace& copy);

	bool isFlex;
	UINT space, actualSpace;
};

typedef enum class overflow_layout_approach
{
	ola_contain, // The layout will simply place a scroller between itself and the child control requesting more space (Default)
	ola_vertical // The layout will expand Vertically upon encountering a child control that needs more space. 
};

/**
 * Class: TLayout
 * Purpose: Adds one dimension of Structure to the TRandomLayout
 */
class _ANA_WIDGET TLayout : public TRandomLayout
{
	friend class AnafacePage;
public:

	virtual bool onCreate(const RECT_F& loc, TrecPointer<TFileShell> d)override;

	TrecPointer<TVariable> SaveToVariable() override;
	TString VariableName()override;

	TLayout(TrecPointer<DrawingBoard> drawingBoard, TDataMap<TDataMap<TString>> styles);

	virtual ~TLayout();

	virtual void Draw(TrecPointer<TVariable> object) override;

	ag_msg virtual void OnResize(RECT_F& newLoc, UINT nFlags, TDataArray<EventID_Cred>& eventAr) override;

	ag_msg virtual bool OnScroll(bool, const TPoint& point, const TPoint& direction, TDataArray<EventID_Cred>&) override;

	virtual bool AddPage(TrecPointer<TPage> page, UINT row, UINT col, bool doOverride = false)override;

	virtual bool RemovePage(UINT row, UINT col)override;

	virtual void GetControlCapacity(UINT& row, UINT& col) override;

	virtual TrecPointer<TPage> GetPage(UINT row, UINT col) override;

	virtual int AddRow(UINT space, bool isFlex);

	virtual int AddCol(UINT space, bool isFlex);

	virtual void InjectScrollerPage(const RECT_F& bounds, const RECT_F& needs, TrecPointer<TPage> page)override;

protected:

	overflow_layout_approach overflowResponse;

	bool ParseDimensions(TDataArray<LayoutSpace>& dimension, UINT totalSpace);

	/**
	 * Spacing Information of the Dimension
	 */
	TDataArray<LayoutSpace> primDem;

	/**
	 * The Dimension direction to aim the Primary dimension of layout (vertical if true)
	 */
	bool primaryStack;
};

