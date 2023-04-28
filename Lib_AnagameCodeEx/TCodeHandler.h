#pragma once
#include <PageTypes.h>
#include <TTextInput.h>
class TCodeHandler :
	public TFileEventHandler
{
	TrecPointer<TTextInput> input;
public:
	virtual void Initialize(TrecPointer<TPage> page) override;

	virtual bool OnChar(bool fromChar, UINT nChar, UINT nRepCnt, UINT nFlags, message_output* mOut) override;

	virtual void SetSelf(TrecPointer<TPage::EventHandler> handleSelf) override;

	virtual TrecPointer<TPage> GetPage() override;

	virtual void HandleEvents(TDataArray<TPage::EventID_Cred>& eventAr) override;

	virtual void SaveFile(TString&) override;

	virtual void LoadFile(TString&) override;

	virtual TrecPointer<TVariable> GetData() override;

	virtual TString SubmitInfo(TrecPointer<TVariable>) override;
};