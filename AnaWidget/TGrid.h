#pragma once
#include "TLayout.h"

/**
 * Class: TGrid
 * Purpose: Adds a second dimension for layout to the TLayout control
 */
class _ANA_WIDGET TGrid :
	public TLayout
{
public:

	virtual bool onCreate(const RECT_F& loc, TrecPointer<TFileShell> d)override;

	TGrid(TrecPointer<DrawingBoard> drawingBoard, TDataMap<TDataMap<TString>> styles);

	virtual ~TGrid();

	virtual void Draw(TrecPointer<TVariable> object) override;

	ag_msg virtual void OnResize(RECT_F& newLoc, UINT nFlags, TDataArray<EventID_Cred>& eventAr) override;

	ag_msg virtual bool OnScroll(bool, const TPoint& point, const TPoint& direction, TDataArray<EventID_Cred>&) override;

	virtual bool AddPage(TrecPointer<TPage> page, UINT row, UINT col, bool doOverride = false)override;

	virtual bool RemovePage(UINT row, UINT col)override;

	virtual void GetControlCapacity(UINT& row, UINT& col) override;

	virtual int AddRow(UINT space, bool isFlex);

	virtual int AddCol(UINT space, bool isFlex);


	virtual TrecPointer<TPage> GetPage(UINT row, UINT col) override;

	RECT_F GetLocationBySection(UINT row, UINT col);

protected:

	void RefreshChildControls(bool newRow);

	/**
	 * Spacing Information of the second Dimension
	 */
	TDataArray<LayoutSpace> secDem;
};

