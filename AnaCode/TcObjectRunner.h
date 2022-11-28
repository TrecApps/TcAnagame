#pragma once
#include "TcProcedureRunner.h"
#include "TcStatement.h"
class TcObjectRunner :
    public TcProcedureRunner
{
    TrecPointer<TcObjectProcedure> procedure;
    TrecPointer<TcOperator> theOps;
    UINT statementCounter;
    USHORT nextStack;
    typedef void (TcObjectRunner::*TcObjectStatmentHandler)(ReturnObject& ret, TcStatement& statement);

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
    void ProcessRegular(ReturnObject& ret, TcStatement& statement);

    void ProcessIf(ReturnObject& ret, TcStatement& statement);
    void ProcessNIf(ReturnObject& ret, TcStatement& statement);
    void ProcessWhile(ReturnObject& ret, TcStatement& statement);
    void ProcessUntil(ReturnObject& ret, TcStatement& statement);
    void ProcessDoWhile(ReturnObject& ret, TcStatement& statement);
    void ProcessFor1(ReturnObject& ret, TcStatement& statement);
    void ProcessFor3(ReturnObject& ret, TcStatement& statement);

    void ProcessSwitch(ReturnObject& ret, TcStatement& statement);
    void ProcessCase(ReturnObject& ret, TcStatement& statement);
    void ProcessSwitchDefault(ReturnObject& ret, TcStatement& statement);

    void ProcessDeclareNew(ReturnObject& ret, TcStatement& statement);
    void ProcessDeclareNewConst(ReturnObject& ret, TcStatement& statement);
    void ProcessDeclareNewHoist(ReturnObject& ret, TcStatement& statement);

    void ProcessTry(ReturnObject& ret, TcStatement& statement);
    void ProcessCatch(ReturnObject& ret, TcStatement& statement);
    void ProcessFinally(ReturnObject& ret, TcStatement& statement);
    void ProcessThrow(ReturnObject& ret, TcStatement& statement);

    void ProcessReturn(ReturnObject& ret, TcStatement& statement);
    void ProcessBreak(ReturnObject& ret, TcStatement& statement);
    void ProcessContinue(ReturnObject& ret, TcStatement& statement);
    void ProcessYeild(ReturnObject& ret, TcStatement& statement);
    void ProcessGoTo(ReturnObject& ret, TcStatement& statement);
    void ProcessGoToTarget(ReturnObject& ret, TcStatement& statement);

    // Helper Methods
    bool doBlockReturn(ReturnObject& ret, TcStatement& statement);
    TrecPointer<TcProcedureRunner> GetProcedureRunner();
    TrecPointer<TcObjectRunner> GenerateBlockRunner(TcStatement& statement);
    void RunBlock(ReturnObject& ret);
};

