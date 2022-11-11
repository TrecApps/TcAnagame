#pragma once
#include <TString.h>
#include "TcProcedureRunner.h"
class TcExpression :
    public TVariable
{
public:
    var_type GetVarType() override;

    virtual void Express(TrecPointer<TcProcedureRunner> runner, TrecPointer<TEnvironment> environment, ReturnObject& ret) = 0;
    virtual bool NeedsResources() = 0;
    virtual bool Append(TrecPointer<TVariable> value) = 0;
    virtual bool CanAssign();
};

class TcAssignableExpression : TcExpression
{
public:
    bool CanAssign()override;

    virtual void Assign(TrecPointer<TVariable> value, ReturnObject& ret) = 0;
};

