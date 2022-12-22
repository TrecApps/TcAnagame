#pragma once
#include "TInstance.h"
#include <TcProcedureRunner.h>
class TDialog :
    public TWindow
{
    friend class TInstance;
    friend class TrecPointerKey;
protected:
    TrecPointer<TVariable> result;
    TrecPointer<TWindow> parent;

    TDialog(TrecPointer<TWindow> parent, GLFWwindow* win);
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
    
};


TrecPointer<TFileShell> OpenLoadFileDialog(const TString& fileInfo, TrecPointer<TDirectory> dir, TString& error);
bool OpenSaveFileDialog(const TString& fileInfo, TrecPointer<TDirectory> dir, const TString& name, TString& error);
TrecPointer<TFileShell> OpenDirectoryDialog(TrecPointer<TDirectory> dir, TString& error);