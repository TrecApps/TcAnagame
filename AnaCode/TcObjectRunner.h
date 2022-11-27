#pragma once
#include "TcProcedureRunner.h"
#include "TcStatement.h"
class TcObjectRunner :
    public TcProcedureRunner
{
    TrecPointer<TcObjectProcedure> procedure;
    UINT statementCounter;
    USHORT nextStack;
    typedef void (TcObjectRunner::*TcObjectStatmentHandler)(ReturnObject& ret);

    TDataArray<TcObjectStatmentHandler> statementHandlers;

public:
    TcObjectRunner(TrecActivePointer<TcObjectProcedure> proc, TrecPointer<TEnvironment> environment, TrecPointer<TcProcedureRunner> parentRunner);
    virtual ~TcObjectRunner() = default;

    void SetIntialVariables(TDataArray<TrecPointer<TVariable>>& params) override;

    void Run(ReturnObject& ret) override;
    void Init();
    void Resume(ReturnObject& ret);

private:

    // Statement Types to Manage
    void ProcessRegular(ReturnObject& ret);

    void ProcessIf(ReturnObject& ret);
    void ProcessNIf(ReturnObject& ret);
    void ProcessWhile(ReturnObject& ret);
    void ProcessUntil(ReturnObject& ret);
    void ProcessDoWhile(ReturnObject& ret);
    void ProcessFor1(ReturnObject& ret);
    void ProcessFor3(ReturnObject& ret);

    void ProcessSwitch(ReturnObject& ret);
    void ProcessCase(ReturnObject& ret);
    void ProcessSwitchDefault(ReturnObject& ret);

    void ProcessDeclareNew(ReturnObject& ret);
    void ProcessDeclareNewConst(ReturnObject& ret);
    void ProcessDeclareNewHoist(ReturnObject& ret);

    void ProcessTry(ReturnObject& ret);
    void ProcessCatch(ReturnObject& ret);
    void ProcessFinally(ReturnObject& ret);
    void ProcessThrow(ReturnObject& ret);

    void ProcessReturn(ReturnObject& ret);
    void ProcessBreak(ReturnObject& ret);
    void ProcessContinue(ReturnObject& ret);
    void ProcessYeild(ReturnObject& ret);
    void ProcessGoTo(ReturnObject& ret);
    void ProcessGoToTarget(ReturnObject& ret);

    // Helper Methods
    void RunBlock(ReturnObject& ret);
};

