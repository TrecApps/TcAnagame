#pragma once
#include <TPage.h>
#include <TControl.h>
class FindTextHandler :
    public TPage::EventHandler
{
    typedef void (FindTextHandler::* FindTextCall)(TrecPointer<TPage> tc, EventArgs ea);
    TDataArray<FindTextCall> calls;
    TDataMap<UINT> events;

    TrecPointerSoft<TPage> page;


    TrecPointerSoft<TControl> replace1;
    TrecPointerSoft<TControl> replace2;

    TString find1;
    TString find2;
    TString find3;

    bool caseSensitive;
    bool wholeWord;
    bool highlightAll;

public:
    FindTextHandler();

    virtual void Initialize(TrecPointer<TPage> page) override;

    virtual bool OnChar(bool fromChar, UINT nChar, UINT nRepCnt, UINT nFlags, message_output* mOut) override;

    virtual void SetSelf(TrecPointer<TPage::EventHandler> handleSelf) override;

    virtual TrecPointer<TPage> GetPage() override;

    virtual void HandleEvents(TDataArray<TPage::EventID_Cred>& eventAr) override;
private:
    void OnFindChange(TrecPointer<TPage> tc, EventArgs ea);
    void OnSetCaseSensitivity(TrecPointer<TPage> tc, EventArgs ea);
    void OnSetWholeWord(TrecPointer<TPage> tc, EventArgs ea);
    void OnSetHighlightAll(TrecPointer<TPage> tc, EventArgs ea);
    void OnSetShowReplace(TrecPointer<TPage> tc, EventArgs ea);
};

