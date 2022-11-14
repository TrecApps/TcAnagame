#pragma once
#include <TString.h>
#include <TPrimitiveVariable.h>
#include "TcProcedureRunner.h"
#include "TcOperator.h"
class TcExpression :
    public TVariable
{
public:
    var_type GetVarType() override;

    virtual void Express(TrecPointer<TcProcedureRunner> runner, TrecPointer<TEnvironment> environment, ReturnObject& ret) = 0;
    virtual bool NeedsResources() = 0;
    virtual bool Append(TrecPointer<TcExpression> value) = 0;
    virtual bool CanAssign();
};

class TcAssignableExpression : public TcExpression
{
public:
    bool CanAssign()override;

    virtual void Assign(TrecPointer<TcProcedureRunner> runner, TrecPointer<TVariable> value, ReturnObject& ret) = 0;
};

class TcVariableExpression : public TcAssignableExpression
{
    TString name;
public:
    TcVariableExpression(const TString& name);

    void Express(TrecPointer<TcProcedureRunner> runner, TrecPointer<TEnvironment> environment, ReturnObject& ret) override;
    bool NeedsResources() override;
    bool Append(TrecPointer<TcExpression> value) override;

    void Assign(TrecPointer<TcProcedureRunner> runner, TrecPointer<TVariable> value, ReturnObject& ret) override;
};

class TcStringExpression : public TcExpression
{
protected:
    TString value;
    TDataArray<TrecPointer<TcExpression>> expressions;
    TString replacer;
public:
    TcStringExpression(const TString& value);
    TcStringExpression(const TString& value, const TString& replacer);

    void Express(TrecPointer<TcProcedureRunner> runner, TrecPointer<TEnvironment> environment, ReturnObject& ret) override;
    bool NeedsResources() override;
    bool Append(TrecPointer<TcExpression> value) override;
};

class TcNumberExpression : public TcExpression
{
protected:
    DoubleLong value;
public:
    TcNumberExpression(DoubleLong value);

    void Express(TrecPointer<TcProcedureRunner> runner, TrecPointer<TEnvironment> environment, ReturnObject& ret) override;
    bool NeedsResources() override;
    bool Append(TrecPointer<TcExpression> value) override;
};


class TcFunctionCallExpression : public TcExpression
{
protected:
    TString functionName;
    TDataArray<TrecPointer<TcExpression>> paramValues;
public:
    TcFunctionCallExpression(const TString& name);

    void Express(TrecPointer<TcProcedureRunner> runner, TrecPointer<TEnvironment> environment, ReturnObject& ret) override;
    bool NeedsResources() override;
    bool Append(TrecPointer<TcExpression> value) override;
};

class TcContainerExpression : public TcExpression
{
    TDataArray<TrecPointer<TcExpression>> paramValues;
public:

    void Express(TrecPointer<TcProcedureRunner> runner, TrecPointer<TEnvironment> environment, ReturnObject& ret) override;
    bool NeedsResources() override;
    bool Append(TrecPointer<TcExpression> value) override;
};

class TcElementExpression : public TcAssignableExpression
{
    TrecPointer<TContainerVariable> container;
    TrecPointer<TcExpression> expression;
    TString key;
public:
    TcElementExpression(TrecPointer<TcExpression> expression);

    void Express(TrecPointer<TcProcedureRunner> runner, TrecPointer<TEnvironment> environment, ReturnObject& ret) override;
    bool NeedsResources() override;
    bool Append(TrecPointer<TcExpression> value) override;

    void Assign(TrecPointer<TcProcedureRunner> runner, TrecPointer<TVariable> value, ReturnObject& ret) override;
};

class TcIncDecExpression : public TcAssignableExpression
{

};

class TcGroupExpression : public TcExpression
{

};