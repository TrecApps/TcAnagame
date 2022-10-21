#pragma once
#include "TControl.h"
#include <TCamera.h>


/*
 * Class: TArenaView
 * Purpose: Holds the Camera for view into a 3D Arena
 *
 * SuperClass: TControl, TCamera
 */
class _ANA_WIDGET TArenaView :
	public TControl//, public TCamera
{
public:

	TArenaView(TrecPointer<DrawingBoard>, TDataMap<TDataMap<TString>> ta, CAMERA_TYPE lookTo = LOOK_TO);

	~TArenaView();

	//TrecPointer<TArenaEngine> getEngine();

	virtual bool onCreate(const RECT_F& loc, TrecPointer<TFileShell> d) override;

	virtual void Draw(TrecPointer<TVariable> object) override;

	ag_msg virtual void OnResize(RECT_F& newLoc, UINT nFlags, TDataArray<EventID_Cred>& eventAr) override;

	ag_msg virtual bool OnScroll(bool, const TPoint& point, const TPoint& direction, TDataArray<EventID_Cred>&) override;

};

