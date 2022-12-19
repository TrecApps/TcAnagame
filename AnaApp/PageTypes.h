#pragma once

#include <HandlerPage.h>
#include "AnaApp.h"

using page_type = enum class page_type
{
	file,			// Page/Handler Combo Holds a file and can load/save data
	file_sup,		// Serves as a Supplement for a File
	main_ribbon,	// Serves as a Ribbon that should persist
	ribbon_sup,		// Serves as a Ribbon Page that can be enabled/disabled according to the active file Page
};

class _ANA_APP TPageEx : public TPage
{
protected:
	TrecPointer<TPage> mainPage;
	TString name;
public:

	TPageEx(TrecActivePointer<TPage> page, const TString& name);
	TrecPointer<TPage> GetPage();
	TString GetName() override;


	virtual page_type GetPageType() = 0;
	virtual bool HandlesEvents() override;

	virtual void Draw(TrecPointer<TVariable> object) override;

	ag_msg virtual void OnRButtonUp(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>&) override;
	ag_msg virtual void OnRButtonDown(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>&) override;
	ag_msg virtual void OnLButtonUp(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>&) override;
	ag_msg virtual void OnLButtonDown(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>&) override;
	ag_msg virtual void OnMouseMove(UINT nFlags, TPoint point, message_output& mOut, TDataArray<EventID_Cred>&) override;
	ag_msg virtual void OnLButtonDblClk(UINT nFlags, TPoint point, message_output& mOut, TDataArray<EventID_Cred>& eventAr) override;
	ag_msg virtual void OnResize(RECT_F& newLoc, UINT nFlags, TDataArray<EventID_Cred>& eventAr) override;
	ag_msg virtual bool OnDestroy() override;
	ag_msg virtual bool OnScroll(bool fromBars, const TPoint& point, const TPoint& direction, TDataArray<EventID_Cred>&) override;
};

class _ANA_APP TRibbonPage : public TPageEx
{
	bool isMain;
public:
	TRibbonPage(TrecActivePointer<TPage> page, const TString& name, bool isMain = false);
	page_type GetPageType() override;
};