#pragma once
#include "TRandomLayout.h"
#include <TObjectNode.h>

/**
 * Class: TDataNodeControl
 * Purpose: Support for depicting a tree-based system on Anaface 2
 *
 * Note: Based heavily on the TTreeDataBind class in the original Anaface
 */
class _ANA_WIDGET TDataNodeControl :
	public TRandomLayout
{
protected:

	/**
	 * The root node that starts the tree this control goes down
	 */
	TrecPointer<TObjectNode> mainNode;
	/**
	 * Used to track whether a node is selected and if the tick is selected
	 */
	bool isNodeSelected, isTickSelected;
	/**
	 * Whether expansion needs to be blocked
	 */
	bool blockExpansion;
	/**
	 * Used to trck the initial click down so when the user clicks up, the node can be determined
	 */
	UINT nodeSelected;

	/**
	 * Used to know when to highlight a given node
	 */
	UINT highlightNodeSelected;

	/**
	 * The color to use for Highlighing
	 */
	TrecPointer<TColorBrush> nodeBrush;

	/**
	 * Brush that paints the outside of the arrow
	 */
	TrecPointer<TColorBrush> outerBrush;
	/**
	 * Brush that paint the inside of the arrow
	 */
	TrecPointer<TColorBrush> innerBrush;

public:
	TrecPointer<TObjectNode> GetNode();


	TDataNodeControl(TrecPointer<DrawingBoard> board, TDataMap<TDataMap<TString>> ta);

	void SetNode(TrecPointer<TObjectNode> newNode);

	ag_msg virtual bool OnScroll(bool, const TPoint& point, const TPoint& direction, TDataArray<EventID_Cred>&) override;

	ag_msg virtual void OnLButtonDblClk(UINT nFlags, TPoint point, message_output& mOut, TDataArray<EventID_Cred>&) override;

	ag_msg virtual void OnResize(RECT_F& newLoc, UINT nFlags, TDataArray<EventID_Cred>& eventAr) override;

	ag_msg virtual void OnLButtonUp(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>&) override;

	ag_msg virtual void OnLButtonDown(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>&) override;

	bool onCreate(const RECT_F& r, TrecPointer<TFileShell> d) override;

	virtual void Draw(TrecPointer<TVariable> object) override;

	virtual RECT_F GetArea();
};

