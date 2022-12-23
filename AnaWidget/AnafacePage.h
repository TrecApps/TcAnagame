#pragma once
#include <TDataMap.h>
#include "HandlerPage.h"
#include "TControl.h"

typedef struct LayoutData
{
	bool isLayout;
	int col, row, endCol, endRow;
}LayoutData;

/**
 * Class: AnafacePage
 * Purpose: Serves as the root page for Anaface 2 - UIs and holds an Event Handler
 */
class _ANA_WIDGET AnafacePage : public HandlerPage
{
public:
	/**
	 * Method: AnafacePage::AnafacePage
	 * Purpose: Constructor
	 * Parameters: TrecPointer<DrawingBoard> board - the drawing board to draw against
	 * Returns: new AnafacePage Object
	 */
	AnafacePage(TrecPointer<DrawingBoard> board, TrecPointer<EventHandler> handler);

	virtual bool HandlesEvents() override;

	void Create(const RECT_F& loc);

	virtual TString PrepPage(TrecPointer<TFileShell> file, TrecPointer<EventHandler> handler) override;

	virtual void Draw(TrecPointer<TVariable> object) override;

	ag_msg virtual void OnRButtonUp(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>&) override;

	ag_msg virtual void OnRButtonDown(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>&) override;

	ag_msg virtual void OnLButtonUp(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>&) override;

	ag_msg virtual void OnLButtonDown(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>&) override;

	ag_msg virtual void OnMouseMove(UINT nFlags, TPoint point, message_output& mOut, TDataArray<EventID_Cred>&) override;

	ag_msg virtual void OnLButtonDblClk(UINT nFlags, TPoint point, message_output& mOut, TDataArray<EventID_Cred>&) override;

	ag_msg virtual void OnResize(RECT_F& newLoc, UINT nFlags, TDataArray<EventID_Cred>& eventAr) override;

	ag_msg virtual bool OnDestroy() override;

	ag_msg virtual bool OnScroll(bool, const TPoint& point, const TPoint& direction, TDataArray<EventID_Cred>&) override;

	virtual void InjectScrollerPage(const RECT_F& bounds, const RECT_F& needs, TrecPointer<TPage> page) override;

	/**
	 * Method: AnafacePage::GetRootControl
	 * Purpose: Retrieves the Root Control for operations
	 * Parameters: void
	 * Returns: TrecPointer<TPage> page - the page generated
	 */
	TrecPointer<TPage> GetRootControl();


	/**
	 * Method: TPage::GetHandler
	 * Purpose: Retrieves the Handler he page holds (if the page is holding a handler)
	 * Parameters: void
	 * Returns: TrecPointer<EventHandler> - the handler the page holds
	 *
	 * Attributes: override

	 */
	virtual TrecPointer<EventHandler> GetHandler() override;

	TrecPointer<TControl> GetControlById(const TString& id);

protected:


	/**
	 * The root Page (TControl)
	 */
	TrecPointer<TPage> rootPage;

	/**
	 * Handles Events
	 */
	TrecPointer<EventHandler> handler;

	/**
	 * List of Styles
	 */
	TDataMap<TDataMap<TString>> styles;

	/**
	 * The directory where the source file originates
	 */
	TrecPointer<TFileShell> directory;


protected:
	// Methods used for construction

	void HandleNode(const TString& name, TString& result, TrecPointer<TVariable> var, TrecPointer<TPage>& curPage, LayoutData& ld);

	TrecPointer<TPage> HandleControl(const TString& name, TString& result, TrecPointer<TVariable> var, LayoutData& ld);

	void HandleAttributes(TString& result, TrecPointer<TPage>& curPage, TrecPointer<TVariable> var, LayoutData& ld);

	void HandleDimensionList(bool row, TString& result, TrecPointer<TPage>& curPage, TrecPointer<TVariable> var);

	//void 
};

