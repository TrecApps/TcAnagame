#pragma once
#include "TGadget.h"

typedef union number_type_control
{
	int i;
	float f;
}number_type_control;

class TextIncrementControl
{
public:
	TextIncrementControl();

	number_type_control currentVal;
	number_type_control increment;
	bool useFloat;

	void Increment();
	void Decrement();

	TString GetString();

};


class _ANA_WIDGET TTextInput :
	public TGadget
{
public:

	TrecPointer<TTextIntercepter> GetInterceptor();

	TTextInput(TrecPointer<DrawingBoard> rt, TDataMap<TDataMap<TString>> ta);

	virtual ~TTextInput();

	bool onCreate(const RECT_F& loc, TrecPointer<TFileShell> d)override;

	virtual void Draw(TrecPointer<TVariable> object) override;

	ag_msg virtual void OnLButtonUp(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>&) override;

	ag_msg virtual void OnLButtonDown(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& args) override;

	ag_msg virtual void OnMouseMove(UINT nFlags, TPoint point, message_output& mOut, TDataArray<EventID_Cred>&) override;

	ag_msg virtual void OnResize(RECT_F& newLoc, UINT nFlags, TDataArray<EventID_Cred>& eventAr) override;

	ag_msg virtual void OnChar(UINT character, UINT count, TDataArray<EventID_Cred>& cred) override;

	bool SetNumericText(float f);

	void SetText(const TString& text);

	TString GetText();

	void LockText(bool doLock);

	void SetPasswordMode(bool b);

	bool IsEnabled();

protected:

	virtual void SetUpTextElement() override;

	bool useNumber, usePassword, passwordPeek, editEnabled, useNumBoxes, showPassword;

	bool textLock;

	TextIncrementControl numberControl;

	HWND windowHandle;

};

