#pragma once
#include <TPage.h>
#include <TIdeWindow.h>
class MainPageIdeHandler :
    public TPage::EventHandler
{
	TrecPointerSoft<TIdeWindow> window;

	typedef void (MainPageIdeHandler::* LayoutCall)(TrecPointer<TPage> tc, EventArgs ea);
	TDataArray<LayoutCall> calls;
	TDataMap<UINT> events;

	TrecPointerSoft<TControl> resourceColumn;
	TrecPointerSoft<TControl> projectName;
public:
	MainPageIdeHandler();


	virtual void Initialize(TrecPointer<TPage> page) override;

	virtual bool OnChar(bool fromChar, UINT nChar, UINT nRepCnt, UINT nFlags, message_output* mOut) override;

	virtual void SetSelf(TrecPointer<TPage::EventHandler> handleSelf) override;

	virtual TrecPointer<TPage> GetPage() override;

	virtual void HandleEvents(TDataArray<TPage::EventID_Cred>& eventAr) override;

private:
	// Found on the Home Tab
	void OnLoadNewSolution(TrecPointer<TPage> tc, EventArgs ea);
	void OnSaveFile(TrecPointer<TPage> tc, EventArgs ea);
	void OnSaveAllFiles(TrecPointer<TPage> tc, EventArgs ea);
	void OnNewResource(TrecPointer<TPage> tc, EventArgs ea);
	void OnImportFile(TrecPointer<TPage> tc, EventArgs ea);
	void OnPrint(TrecPointer<TPage> tc, EventArgs ea);
};

