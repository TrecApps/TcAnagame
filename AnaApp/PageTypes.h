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

class _ANA_APP TFileEventHandler : public TPage::EventHandler
{
protected:
	TrecPointer<TFileShell> file;
public:

	void SetDataFile(TrecPointer<TFileShell> file)override;

	virtual void SaveFile(TString& ret) = 0;

	virtual void LoadFile(TString& ret) = 0;

	virtual TrecPointer<TVariable> GetData() = 0;

	virtual TString SubmitInfo(TrecPointer<TVariable>) = 0;


};

class _ANA_APP TFilePage : public TPageEx
{
	TrecPointer<TFileEventHandler> fileEventHandler;
	TString type;
public:

	TFilePage(TrecActivePointer<TPage> page, const TString& name, const TString& type);

	page_type GetPageType() override;

	void SetFile(TrecPointer<TFileShell> file);

	void SaveFile(TString&);

	void LoadFile(TString&);

	TrecPointer<TVariable> GetData();

	TString SubmitInfo(TrecPointer<TVariable>);

	void OnFocus();
	void OnNonFocus();
};

class _ANA_APP TFileSupHandler : public TPage::EventHandler
{
public:
	virtual void GetFilePageTypes(TDataArray<TString>& types) = 0;

};

class _ANA_APP TFileSupPage : public TPageEx
{
	TrecPointer<TFileSupHandler> fileSupHandler;
public:
	TFileSupPage(TrecActivePointer<TPage> page, const TString& name);
	bool GetFilePageTypes(TDataArray<TString>& types);
};