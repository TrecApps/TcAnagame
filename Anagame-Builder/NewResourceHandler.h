#pragma once
#include <TPage.h>
#include <TIdeWindow.h>
#include <TDialog.h>
class NewResourceHandler :
    public TPage::EventHandler
{
	TrecPointerSoft<TIdeWindow> window;

	TrecPointer<TDialog> dialogWindow;

	typedef void (NewResourceHandler::* LayoutCall)(TrecPointer<TPage> tc, EventArgs ea);
	TDataArray<LayoutCall> calls;
	TDataMap<UINT> events;
public:
	NewResourceHandler();


	virtual void Initialize(TrecPointer<TPage> page) override;

	virtual bool OnChar(bool fromChar, UINT nChar, UINT nRepCnt, UINT nFlags, message_output* mOut) override;

	virtual void SetSelf(TrecPointer<TPage::EventHandler> handleSelf) override;

	virtual TrecPointer<TPage> GetPage() override;

	virtual void HandleEvents(TDataArray<TPage::EventID_Cred>& eventAr) override;

private:

	void Close();

	void OnCancel(TrecPointer<TPage> tc, EventArgs ea);
	void OnOkay(TrecPointer<TPage> tc, EventArgs ea);
	void OnUpdateName(TrecPointer<TPage> tc, EventArgs ea);
	void OnResourceSelect(TrecPointer<TPage> tc, EventArgs ea);
};

