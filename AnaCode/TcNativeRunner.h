#pragma once
#include "TcProcedureRunner.h"

typedef void (*NativeFunction)(TDataArray<TrecPointer<TVariable>>& params, TrecPointer<TEnvironment> env, ReturnObject& ret);

class TcNativeRunner :
    public TcProcedureRunner
{
    NativeFunction function;
    TDataArray<TrecPointer<TVariable>> initialVariables;
public:
    TcNativeRunner(NativeFunction f, TrecPointer<TEnvironment> environment, TrecPointer<TcProcedureRunner> parentRunner);
    void SetIntialVariables(TDataArray<TrecPointer<TVariable>>& params) override;

    void Run(ReturnObject& ret);
};

