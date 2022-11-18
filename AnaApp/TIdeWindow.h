#pragma once
#include "TWindow.h"
#include <TEnvironment.h>
#include "AnagameEnvironment.h"

using ide_property = enum class ide_property
{
    drag_management, // What Page the Window should seek out whenever the user drags a tab around
    main_page_space, 
};

class TIdeWindow :
    public TWindow
{
protected:
    TrecPointer<BasicAnagameEnvironment> environment;

    TrecPointer<TPage> dragManager;
    UINT mainPageSpace;



public:
    TIdeWindow(GLFWwindow* window);

    void SetMainPage(TrecPointer<TPage> mainPage) override;

    void SetProject(TrecActivePointer<AGProjectEnvironment> project);

    void SetIdeProperty(ide_property, const TString& prop);
};

