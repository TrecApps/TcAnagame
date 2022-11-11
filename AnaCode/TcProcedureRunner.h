#pragma once
#include "AnaCode.h"
#include <TcRunner.h>
#include <TDataMap.h>
#include <TEnvironment.h>

class _ANA_CODE TcVariableHolder
{
public:
    bool mut;                       // Is this variable mutable
    TString type;                   // The type of Variable this should be (empty string means anything)
    TrecPointer<TVariable> value;   // The Value held by the Variable

    TcVariableHolder();

    TcVariableHolder(const TcVariableHolder& copy);

    TcVariableHolder(bool mut, const TString& type, TrecPointer<TVariable> value);
};

class TcProcedureRunner :
    public TcRunner
{
protected:
    TDataMap<TcVariableHolder> variables;
    TDataArray<TString> paramNames, paramTypes;

    TrecPointer<TEnvironment> environment;
    TrecPointerSoft<TcProcedureRunner> parentRunner;
public:
    TcProcedureRunner(TrecPointer<TEnvironment> environment, TrecPointer<TcProcedureRunner> parentRunner);

    virtual void SetIntialVariables(TDataArray<TrecPointer<TVariable>>& params) = 0;

    void SetParamNames(TDataArray<TString>& paramNames);

    void SetParamTypes(TDataArray<TString>& paramTypes);
};

