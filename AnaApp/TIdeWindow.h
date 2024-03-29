#pragma once
#include "TWindow.h"
#include <TEnvironment.h>
#include "AnagameEnvironment.h"
#include <TSwitchControl.h>
#include "PageTypes.h"
#include "TInstance.h"

using ide_property = enum class ide_property
{
    drag_management, // What Page the Window should seek out whenever the user drags a tab around
    main_page_space, 
};

class _ANA_APP TIdeWindow :
    public TWindow
{
protected:
    TrecPointer<BasicAnagameEnvironment> environment;

    TrecPointer<TPage> dragManager;
    UINT mainPageSpace;

    TrecPointer<TSwitchControl> ribbonTabs;

    void SaveIde();

    void LoadLayout(const TString& layoutFile);

    RECT_F setTo;
    bool doResize;

    TDataArray<TrecPointerSoft<TPage>> registeredPages;
    TrecPointerSoft<TPage> currentFilePage;

public:
    // Inherited From TWindow
    void OnResize(int w, int h) override;

    void PrepResize();

    void SetFocusObject(TrecPointer<TObject> focusObject) override;


    // IDE Specific Methods

    void PrepResource(TrecPointer<TJsonVariable> resourceData);

    TIdeWindow(GLFWwindow* window);

    void SetMainPage(TrecPointer<TPage> mainPage) override;

    bool PrepProject(const TProjectData& projectData);

    bool SetProject(TrecActivePointer<AGProjectEnvironment> project);

    void SetIdeProperty(ide_property, const TString& prop);

    void ActivateRibbons(const TDataArray<TString>& names);

    UINT AddRibbon(TrecActivePointer<TPageEx> ribbon);

    void RemoveRibbon(const TString& ribbonName);

    TrecPointer<BasicAnagameEnvironment> GetEnvironment();

    // IDE Page Management
    void SaveCurrent();
    void SaveAll();
};

