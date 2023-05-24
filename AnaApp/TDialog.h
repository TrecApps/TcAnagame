#pragma once
#include "TInstance.h"
#include <TcProcedureRunner.h>

class _ANA_APP TDialog :
    public TWindow
{
    friend class TInstance;
    friend class TrecPointerKey;
protected:
    TrecPointer<TVariable> result;
    TrecPointer<TWindow> parent;

    TDialog(TrecPointer<TWindow> parent, GLFWwindow* win, VkPhysicalDevice anagameVulkanDevice, VkInstance instance);
    TrecPointer<TcProcedureRunner> closeCallback;
    t_dialog_modal_mode mode;
    bool dialogRunning;
public:

    void SetModalMode(t_dialog_modal_mode newMode);
    void SetCloseCallBack(TrecPointer<TcProcedureRunner> runner);
    TrecPointer<TVariable> GetResult();
    void SetResult(TrecPointer<TVariable> result);
    bool Close()override;

    bool RunModal();

    TrecPointer<TWindow> GetParentWindow();
    
};


TrecPointer<TFileShell> _ANA_APP OpenLoadFileDialog(const TString& fileInfo, TrecPointer<TDirectory> dir, TString& error);
bool _ANA_APP OpenSaveFileDialog(const TString& fileInfo, TrecPointer<TDirectory> dir, const TString& name, TString& error);
TrecPointer<TFileShell> _ANA_APP OpenDirectoryDialog(TrecPointer<TDirectory> dir, TString& error);