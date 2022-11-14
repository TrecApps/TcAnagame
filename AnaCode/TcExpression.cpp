#include "TcExpression.h"
#include <TContainerVariable.h>

var_type TcExpression::GetVarType()
{
    return var_type::expression;
}

bool TcExpression::CanAssign()
{
    return false;
}

bool TcAssignableExpression::CanAssign()
{
    return true;
}

TcVariableExpression::TcVariableExpression(const TString& name)
{
    this->name.Set(name);
}

void TcVariableExpression::Express(TrecPointer<TcProcedureRunner> runner, TrecPointer<TEnvironment> environment, ReturnObject& ret)
{
    if (!runner.Get())
    {
        ret.returnCode = ret.ERR_BROKEN_REF;
        ret.errorMessage.Format(L"Variable expression '%ws' was not provided a Runner for Variable Expression!", name.GetConstantBuffer().getBuffer());
        return;
    }

    TcVariableHolder holder;
    bool result = runner->GetVariable(name, holder);

    if (!result)
    {
        ret.returnCode = ret.ERR_UNDEFINED;
        ret.errorMessage.Format(L"Variable expression '%ws' not found!", name.GetConstantBuffer().getBuffer());
    }
    else
        ret.errorObject = holder.value;
}

bool TcVariableExpression::NeedsResources()
{
    return true;
}

bool TcVariableExpression::Append(TrecPointer<TcExpression> value)
{
    return false;
}

void TcVariableExpression::Assign(TrecPointer<TcProcedureRunner> runner, TrecPointer<TVariable> value, ReturnObject& ret)
{
    if (!runner.Get())
    {
        ret.returnCode = ret.ERR_BROKEN_REF;
        ret.errorMessage.Format(L"Variable expression '%ws' was not provided a Runner for Variable Expression!", name.GetConstantBuffer().getBuffer());
        return;
    }

    if (!runner->UpdateVariable(name, L"", value))
    {
        ret.returnCode = ret.ERR_UNDEFINED;
        ret.errorMessage.Format(L"Variable expression '%ws' not found, or was of an Incompatible Type!", name.GetConstantBuffer().getBuffer());
    }
}

TcStringExpression::TcStringExpression(const TString& value)
{
    this->value.Set(value);
}

TcStringExpression::TcStringExpression(const TString& value, const TString& replacer)
{
    this->value.Set(value);
    this->replacer.Set(replacer);
}

void TcStringExpression::Express(TrecPointer<TcProcedureRunner> runner, TrecPointer<TEnvironment> environment, ReturnObject& ret)
{
    if (!replacer.GetSize())
        ret.errorObject = TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(value);
    else
    {
        TString tempValue(value);
        int replacerIndex = tempValue.Find(replacer);
        for (UINT Rust = 0; replacerIndex != -1 && Rust < expressions.Size() && !ret.returnCode; Rust++)
        {
            expressions[Rust]->Express(runner, environment, ret);
            if (ret.returnCode)
                return;

            TString val;
            if (ret.errorObject.Get())
            {
                auto strVal = TrecPointerKey::ConvertPointer<TVariable, TStringVariable>(ret.errorObject->ToString());

                if (!strVal.Get())
                {
                    ret.returnCode = ret.ERR_INTERNAL;
                    ret.errorMessage.Set(L"Variable detected that does not return a string!");
                    return;
                }
                val.Set(strVal->GetString());
            }

            tempValue.Delete(replacerIndex, replacer.GetSize());
            tempValue.Insert(replacerIndex, val);
            replacerIndex += val.GetSize();
            replacerIndex = tempValue.Find(replacer, replacerIndex);
        }

        ret.errorObject = TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(tempValue);
    }
}

bool TcStringExpression::NeedsResources()
{
    for (UINT Rust = 0; Rust < this->expressions.Size(); Rust++)
    {
        if (expressions[Rust]->NeedsResources())
            return true;
    }
    return false;
}

bool TcStringExpression::Append(TrecPointer<TcExpression> value)
{
    if (replacer.GetSize() && value.Get())
    {
        expressions.push_back(value);
        return true;
    }
    return false;
}

TcNumberExpression::TcNumberExpression(DoubleLong value)
{
    this->value = value;
}

void TcNumberExpression::Express(TrecPointer<TcProcedureRunner> runner, TrecPointer<TEnvironment> environment, ReturnObject& ret)
{
    ret.errorObject.Nullify();
    switch (value.type)
    {
    case double_long::dl_double:
        ret.errorObject = TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TPrimitiveVariable>(value.value.d);
        break;
    case double_long::dl_sign:
        ret.errorObject = TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TPrimitiveVariable>(value.value.s);
        break;
    case double_long::dl_unsign:
        ret.errorObject = TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TPrimitiveVariable>(value.value.u);
        break;
    default:
        ret.returnCode = ret.ERR_BROKEN_REF;
        ret.errorMessage.Set(L"Value Provided was broken");
    }
}

bool TcNumberExpression::NeedsResources()
{
    return false;
}

bool TcNumberExpression::Append(TrecPointer<TcExpression> value)
{
    return false;
}

TcFunctionCallExpression::TcFunctionCallExpression(const TString& name)
{
    this->functionName.Set(name);
}

void TcFunctionCallExpression::Express(TrecPointer<TcProcedureRunner> runner, TrecPointer<TEnvironment> environment, ReturnObject& ret)
{
    TrecPointer<TcProcedureRunner> call;

    TcVariableHolder var;
    if (ret.errorObject.Get())
    {
        switch (ret.errorObject->GetVarType())
        {
        case var_type::json:
            if (dynamic_cast<TJsonVariable*>(ret.errorObject.Get())->RetrieveField(functionName, var.value))
                call = TrecPointerKey::ConvertPointer<TVariable, TcProcedureRunner>(var.value);
            break;
        case var_type::native_object:
            break;
        }
    }

    if (call.Get())
    {
        call->SetMethodObject(ret.errorObject);
    }
    else if (runner->GetVariable(functionName, var)) {
        call = TrecPointerKey::ConvertPointer<TVariable, TcProcedureRunner>(var.value);
        if (!call.Get())
        {
            ret.errorObject.Nullify();
            ret.returnCode = ret.ERR_BROKEN_REF;
            ret.errorMessage.Format(L"Function expression '%ws' not found!", functionName.GetConstantBuffer().getBuffer());
            return;
        }
        
    }
    else
    {
        ret.errorObject.Nullify();
        ret.returnCode = ret.ERR_BROKEN_REF;
        ret.errorMessage.Format(L"Function expression '%ws' not found!", functionName.GetConstantBuffer().getBuffer());
        return;
    }

    TDataArray<TrecPointer<TVariable>> params;

    for (UINT Rust = 0; Rust < this->paramValues.Size(); Rust++)
    {
        paramValues[Rust]->Express(runner, environment, ret);
        if (ret.returnCode)
            return;

        params.push_back(ret.errorObject);
    }

    call->SetIntialVariables(params);

    call->Run(ret);
}

bool TcFunctionCallExpression::NeedsResources()
{
    return true;
}

bool TcFunctionCallExpression::Append(TrecPointer<TcExpression> value)
{
    if(!value.Get())
        return false;
    paramValues.push_back(value);
    return true;
}

void TcContainerExpression::Express(TrecPointer<TcProcedureRunner> runner, TrecPointer<TEnvironment> environment, ReturnObject& ret)
{
    TrecPointer<TVariable> retValue = TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TArrayVariable>();
    TrecPointer<TArrayVariable> retArray = TrecPointerKey::ConvertPointer<TVariable, TArrayVariable>(retValue);

    for (UINT Rust = 0; Rust < this->paramValues.Size(); Rust++)
    {
        paramValues[Rust]->Express(runner, environment, ret);
        if (ret.returnCode)
            return;
        retArray->Push(ret.errorObject);
    }

    ret.errorObject = retValue;
}

bool TcContainerExpression::NeedsResources()
{
    for (UINT Rust = 0; Rust < paramValues.Size(); Rust++)
    {
        if (paramValues[Rust]->NeedsResources())
            return true;
    }
    return false;
}

bool TcContainerExpression::Append(TrecPointer<TcExpression> value)
{
    if (!value.Get())
        return false;
    paramValues.push_back(value);
    return true;
}

TcElementExpression::TcElementExpression(TrecPointer<TcExpression> expression)
{
    this->expression = expression;
}

void TcElementExpression::Express(TrecPointer<TcProcedureRunner> runner, TrecPointer<TEnvironment> environment, ReturnObject& ret)
{
    this->container = TrecPointerKey::ConvertPointer<TVariable, TContainerVariable>(ret.errorObject);
    if (!container.Get())
    {
        ret.returnCode = ret.ERR_BROKEN_REF;
        ret.errorMessage.Set(L"Attempt to Access Element of non-conteiner variable!");
        return;
    }

    expression->Express(runner, environment, ret);
    if (ret.returnCode)
        return;

    key.Set(L"null");

    if (ret.errorObject.Get())
    {
        TrecPointer<TStringVariable> keyVar = TrecPointerKey::ConvertPointer<TVariable, TStringVariable>(
            ret.errorObject->ToString()
            );

        if (keyVar.Get())
            key.Set(keyVar->GetString());
    }

    TrecPointer<TJsonVariable> jsonVariable = TrecPointerKey::ConvertPointer<TContainerVariable, TJsonVariable>(container);
    TrecPointer<TArrayVariable> arrayVariable = TrecPointerKey::ConvertPointer<TContainerVariable, TArrayVariable>(container);

    if (jsonVariable.Get())
    {
        if (!jsonVariable->RetrieveField(key, ret.errorObject))
            ret.errorObject.Nullify();
    }
    else if (arrayVariable.Get())
    {
        UINT index = 0;
        if (!TString::ConvertStringToUint(key, index) || arrayVariable->GetValueAt(index, ret.errorObject))
            ret.errorObject.Nullify();
    }
    else
        ret.errorObject.Nullify();
}

bool TcElementExpression::NeedsResources()
{
    return true;
}

bool TcElementExpression::Append(TrecPointer<TcExpression> value)
{
    return false;
}

void TcElementExpression::Assign(TrecPointer<TcProcedureRunner> runner, TrecPointer<TVariable> value, ReturnObject& ret)
{
    if (!container.Get())
    {
        ret.returnCode = ret.ERR_BROKEN_REF;
        ret.errorMessage.Set(L"Attempt to Access Element of non-conteiner variable!");
        return;
    }

    TrecPointer<TJsonVariable> jsonVariable = TrecPointerKey::ConvertPointer<TContainerVariable, TJsonVariable>(container);
    TrecPointer<TArrayVariable> arrayVariable = TrecPointerKey::ConvertPointer<TContainerVariable, TArrayVariable>(container);

    if (jsonVariable.Get())
    {
        jsonVariable->SetField(key, value, true);
    }
    else if (arrayVariable.Get())
    {
        UINT index = 0;
        if (!TString::ConvertStringToUint(key, index))
        {
            ret.returnCode = ret.ERR_IMPROPER_TYPE;
            ret.errorMessage.Set(L"Attempt to Access Element of Array using non-numeric value");
            return;
        }
        arrayVariable->Update(value, index);
    }
    else
        ret.errorObject.Nullify();
}
