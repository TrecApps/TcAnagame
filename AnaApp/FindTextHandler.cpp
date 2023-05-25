#include "FindTextHandler.h"

TString fr_FindChange(L"OnFindChange");
TString fr_SetCaseSensitivity(L"OnSetCaseSensitivity");
TString fr_SetWholeWord(L"OnSetWholeWord");
TString fr_SetHighlightAll(L"OnSetHighlightAll");
TString fr_SetShowReplace(L"OnSetShowReplace");
//TString fr_Print(L"OnPrint");

FindTextHandler::FindTextHandler()
{
	caseSensitive = highlightAll = wholeWord = false;

	events.addEntry(fr_FindChange, 0);
	calls.push_back(&FindTextHandler::OnFindChange);

	events.addEntry(fr_SetCaseSensitivity, 1);
	calls.push_back(&FindTextHandler::OnSetCaseSensitivity);

	events.addEntry(fr_SetWholeWord, 2);
	calls.push_back(&FindTextHandler::OnSetWholeWord);

	events.addEntry(fr_SetHighlightAll, 3);
	calls.push_back(&FindTextHandler::OnSetHighlightAll);

	events.addEntry(fr_SetShowReplace, 4);
	calls.push_back(&FindTextHandler::OnSetShowReplace);
}

void FindTextHandler::Initialize(TrecPointer<TPage> page)
{
	this->page = TrecPointerKey::SoftFromTrec<>(page);
}

bool FindTextHandler::OnChar(bool fromChar, UINT nChar, UINT nRepCnt, UINT nFlags, message_output* mOut)
{
	return false;
}

void FindTextHandler::SetSelf(TrecPointer<TPage::EventHandler> handleSelf)
{
}

TrecPointer<TPage> FindTextHandler::GetPage()
{
	return TrecPointerKey::TrecFromSoft<>(page);
}

void FindTextHandler::HandleEvents(TDataArray<TPage::EventID_Cred>& eventAr)
{
	int e_id = -1;
	EventArgs ea;
	for (UINT c = 0; c < eventAr.Size(); c++)
	{
		auto tc = eventAr.at(c).args;

		if (!tc.Get()) continue;

		UINT u_id = 0;
		if (!events.retrieveEntry(tc->methodID, u_id))
			continue;
		e_id = u_id;
		// At this point, call the appropriate method
		if (e_id > -1 && e_id < calls.Size())
		{
			// call method
			if (calls[e_id])
				(this->*calls[e_id])(eventAr[c].control, ea);
		}
		eventAr[c].args.Nullify();
	}
}

void FindTextHandler::OnFindChange(TrecPointer<TPage> tc, EventArgs ea)
{
	auto cont = TrecPointerKey::ConvertPointer<TPage, TControl>(tc);
	if (!cont.Get())return;

	TString contId(cont->GetID());
	if (contId.CountFinds(L'1'))
		find1.Set(ea.text);
	if (contId.CountFinds(L'2'))
		find2.Set(ea.text);
	if (contId.CountFinds(L'3'))
		find3.Set(ea.text);

}

void FindTextHandler::OnSetCaseSensitivity(TrecPointer<TPage> tc, EventArgs ea)
{
	caseSensitive = ea.positive;
}

void FindTextHandler::OnSetWholeWord(TrecPointer<TPage> tc, EventArgs ea)
{
	wholeWord = ea.positive;
}

void FindTextHandler::OnSetHighlightAll(TrecPointer<TPage> tc, EventArgs ea)
{
	highlightAll = ea.positive;
}

void FindTextHandler::OnSetShowReplace(TrecPointer<TPage> tc, EventArgs ea)
{
}
