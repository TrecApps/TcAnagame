#pragma once
#include "AnaCode.h"
#include <TcRunner.h>
#include <TDataMap.h>
#include <TEnvironment.h>
#include "TcOperator.h"

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

    TrecPointer<TVariable> object;

    TrecPointer<TcOperatorGroupList> operatorGroupList;
public:
    void SetMethodObject(TrecPointer<TVariable> methodObject);
    TcProcedureRunner(TrecPointer<TEnvironment> environment, TrecPointer<TcProcedureRunner> parentRunner);

    void SetOperatorGroupList(TrecPointer<TcOperatorGroupList> opList);
    TrecPointer<TcOperatorGroupList> GetOperatorGroupList();

    virtual void SetIntialVariables(TDataArray<TrecPointer<TVariable>>& params) = 0;

    void SetParamNames(TDataArray<TString>& paramNames);

    void SetParamTypes(TDataArray<TString>& paramTypes);

    bool GetVariable(const TString& name, TcVariableHolder& value);

    bool UpdateVariable(const TString& name, const TString& type, TrecPointer<TVariable> value, bool appendLocal = true);
};

