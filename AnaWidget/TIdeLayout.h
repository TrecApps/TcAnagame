#pragma once
#include "TRandomLayout.h"
#include "TSwitchControl.h"
#include <TContainerVariable.h>

using ide_section_type = enum class ide_section_type
{
    divider,
    tab,
    page
};

using click_section_event = enum class click_section_event
{
	right_up,
	right_down,
	left_up,
	left_down
};

class _ANA_WIDGET IdeSection : public TCoreObject
{
	friend class TIdeLayout;
protected:
	RECT_F bounds;
public:
	IdeSection();
    virtual ide_section_type GetSectionType() = 0;

	virtual void OnClickEvent(click_section_event clickEvent, UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<TPage::EventID_Cred>& er) = 0;
	virtual void OnMouseMove(UINT nFlags, TPoint point, message_output& mOut, TDataArray<TPage::EventID_Cred>& er) = 0;
	virtual void OnLButtonDblClk(UINT nFlags, TPoint point, message_output& mOut, TDataArray<TPage::EventID_Cred>& args) = 0;
	virtual void OnResize(RECT_F& newLoc, UINT nFlags, TDataArray<TPage::EventID_Cred>& eventAr) = 0;

	virtual void Draw(TrecPointer<TVariable> obj, TrecPointer<TColorBrush> col, float thickness) = 0;
};

class IdeDividerSection : public IdeSection
{
	friend class TIdeLayout;
    bool isVertical;
    float leftTop;
    TrecPointer<IdeSection> first, second;
public:
    ide_section_type GetSectionType() override;

	virtual void OnClickEvent(click_section_event clickEvent, UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<TPage::EventID_Cred>& er) override;
	virtual void OnMouseMove(UINT nFlags, TPoint point, message_output& mOut, TDataArray<TPage::EventID_Cred>& er) override;
	virtual void OnLButtonDblClk(UINT nFlags, TPoint point, message_output& mOut, TDataArray<TPage::EventID_Cred>& args) override;
	virtual void OnResize(RECT_F& newLoc, UINT nFlags, TDataArray<TPage::EventID_Cred>& eventAr) override;

	void Draw(TrecPointer<TVariable> obj, TrecPointer<TColorBrush> col, float thickness) override;

	RECT_F GetSectionArea(bool first);
};

class IdeTabSection : public IdeSection
{
	friend class TIdeLayout;
	TrecPointer<TSwitchControl> tab;
	TString name;
public:
    ide_section_type GetSectionType() override;

	virtual void OnClickEvent(click_section_event clickEvent, UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<TPage::EventID_Cred>& er) override;
	virtual void OnMouseMove(UINT nFlags, TPoint point, message_output& mOut, TDataArray<TPage::EventID_Cred>& er) override;
	virtual void OnLButtonDblClk(UINT nFlags, TPoint point, message_output& mOut, TDataArray<TPage::EventID_Cred>& args) override;
	virtual void OnResize(RECT_F& newLoc, UINT nFlags, TDataArray<TPage::EventID_Cred>& eventAr) override;

	void Draw(TrecPointer<TVariable> obj, TrecPointer<TColorBrush> col, float thickness) override;
};

class IdePageSection : public IdeSection
{
	friend class TIdeLayout;
	TrecPointer<TPage> tab;
public:
	ide_section_type GetSectionType() override;

	virtual void OnClickEvent(click_section_event clickEvent, UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<TPage::EventID_Cred>& er) override;
	virtual void OnMouseMove(UINT nFlags, TPoint point, message_output& mOut, TDataArray<TPage::EventID_Cred>& er) override;
	virtual void OnLButtonDblClk(UINT nFlags, TPoint point, message_output& mOut, TDataArray<TPage::EventID_Cred>& args) override;
	virtual void OnResize(RECT_F& newLoc, UINT nFlags, TDataArray<TPage::EventID_Cred>& eventAr) override;

	void Draw(TrecPointer<TVariable> obj, TrecPointer<TColorBrush> col, float thickness) override;
};

class _ANA_WIDGET TIdeLayout :
    public TRandomLayout
{
protected:
    TrecPointer<TColorBrush> dividerColor;
    float thickness;
    float mouseSensitivity;
    bool dividerMovable;
    bool structureFixed;

	TrecPointer<IdeSection> rootSection;

	TrecPointer<IdeDividerSection> captured;

public:
	virtual bool onCreate(const RECT_F& loc, TrecPointer<TFileShell> d)override;

	TIdeLayout(TrecPointer<DrawingBoard> drawingBoard, TDataMap<TDataMap<TString>> styles);

	virtual ~TIdeLayout();

	virtual void Draw(TrecPointer<TVariable> object) override;

	ag_msg virtual void OnRButtonUp(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>&) override;

	ag_msg virtual void OnRButtonDown(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>&) override;

	ag_msg virtual void OnLButtonUp(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>&) override;

	ag_msg virtual void OnMouseMove(UINT nFlags, TPoint point, message_output& mOut, TDataArray<EventID_Cred>&) override;

	ag_msg virtual void OnLButtonDblClk(UINT nFlags, TPoint point, message_output& mOut, TDataArray<EventID_Cred>& args) override;

	ag_msg virtual void OnLButtonDown(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& args) override;

	ag_msg virtual void OnResize(RECT_F& newLoc, UINT nFlags, TDataArray<EventID_Cred>& eventAr) override;

	ag_msg virtual bool OnScroll(bool, const TPoint& point, const TPoint& direction, TDataArray<EventID_Cred>& args) override;

	virtual bool AddPage(TrecPointer<TPage> page, UINT row, UINT col, bool doOverride = false);

	virtual bool RemovePage(UINT row, UINT col);

	virtual void GetControlCapacity(UINT& row, UINT& col);

	virtual TrecPointer<TPage> GetPage(UINT row, UINT col);

	// Methods for Manipulating Sections
	TrecPointer<IdeSection> GetRootSection();

	void SetUpLayout(TrecPointer<TJsonVariable> variable, bool doOverride = true);

	bool AppendSection(TrecPointer<IdeSection> section, TrecPointer<TPage> page);
	bool AppendSection(TrecPointer<IdeSection> section, TrecPointer<TSwitchControl> tabs);

	TrecPointer<IdeSection> GetFirstSection(TrecPointer<IdeSection> section);
	TrecPointer<IdeSection> GetSecondSection(TrecPointer<IdeSection> section);

	bool InitDivide(bool verticle, bool totalSpace, float dividePoint);
	bool DivideSection(TrecPointer<IdeSection> section, bool verticle, bool totalSpace, float dividePoint);
	bool GetBounds(const TPoint& point, RECT_F& bounds, TrecPointer<IdeSection>& section);
	bool GetBounds(TrecPointer<IdeSection> section, RECT_F& bounds);

protected:
	void HandleVariable(TrecPointer<IdeSection>& section, TrecPointer<TJsonVariable>& variable, const RECT_F& bounds);
	void HandleDividerVariable(TrecPointer<IdeSection>& section, TrecPointer<TJsonVariable>& variable, const RECT_F& bounds);
	void HandleTabVariable(TrecPointer<IdeSection>& section, TrecPointer<TJsonVariable>& variable, const RECT_F& bounds);
	void HandlePageVariable(TrecPointer<IdeSection>& section, TrecPointer<TJsonVariable>& variable, const RECT_F& bounds);
};

