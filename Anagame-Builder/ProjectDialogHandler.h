#pragma once
#include <TPage.h>
#include <TDialog.h>
#include <AnafacePage.h>
#include <TTextInput.h>
#include <TInstance.h>
#include <TContainerVariable.h>
class ProjectDialogHandler :
    public TPage::EventHandler
{
	typedef void (ProjectDialogHandler::* ProjectCall)(TrecPointer<TPage> tc, EventArgs ea);
	TDataArray<ProjectCall> calls;
	TDataMap<UINT> events;
	TrecPointer<TDialog> dialogWindow;

	TrecPointerSoft<TTextInput> inputDirectory;
	TrecPointerSoft<TTextInput> inputName;
	//TrecPointerSoft<TTextInput> inputBuilder;
	//TrecPointerSoft<TTextInput> inputProject;
	TrecPointerSoft<TControl> okayButton;

	TProjectData projectData;

	TrecPointer<TArrayVariable> existingProjects;
	TrecPointer<TArrayVariable> availableProjects;
	
public:
	ProjectDialogHandler();

	/**
	 * Method: EventHandler::Initialize
	 * Purpose: Initializes the Handler so that it has direct Access to certain Controls held by the page
	 * Parameters: TrecPointer<Page> page - page that holds the Controls to latch on to
	 * Returns: void
	 *
	 * Attributes: abstract
	 */
	virtual void Initialize(TrecPointer<TPage> page) override;

	/**
	 * Method: EventHandler::OnChar
	 * Purpose: Handles Character input from the Instance of Anagame
	 * Parameters: bool fromChar - true if it came form On_Char from windows, false if it came from a key_down
	 *				UINT nChar - the character submitted
	 *				UINT nRepCnt - number od times to add the character
	 *				UINT nFlags - unused
	 *				message_output* mOut - result
	 * Returns: bool
	 */
	virtual bool OnChar(bool fromChar, UINT nChar, UINT nRepCnt, UINT nFlags, message_output* mOut) override;

	virtual void SetSelf(TrecPointer<TPage::EventHandler> handleSelf) override;

	virtual TrecPointer<TPage> GetPage() override;

	virtual void HandleEvents(TDataArray<TPage::EventID_Cred>& eventAr) override;

private:
	void Close();
	void AssessOkay();

	void OnDirectorySelect(TrecPointer<TPage> tc, EventArgs ea);
	void OnCancel(TrecPointer<TPage> tc, EventArgs ea);
	void OnOkay(TrecPointer<TPage> tc, EventArgs ea);
	void OnSelectNew(TrecPointer<TPage> tc, EventArgs ea);
	void OnSelectRecent(TrecPointer<TPage> tc, EventArgs ea);
	void OnUpdateName(TrecPointer<TPage> tc, EventArgs ea);
};

