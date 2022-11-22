#include "TcExpression.h"
#include <TContainerVariable.h>
#include <cassert>

var_type TcExpression::GetVarType()
{
    return var_type::expression;
}

bool TcExpression::CanAssign()
{
    return false;
}

TrecPointer<TVariable> TcExpression::Clone()
{
    return TrecPointer<TVariable>();
}

TrecPointer<TVariable> TcExpression::ToString(TrecPointer<TVariable>)
{
    return TrecPointer<TVariable>();
}

TrecPointer<TVariable> TcExpression::ToString()
{
    return TrecPointer<TVariable>();
}

UINT TcExpression::Get4Value()
{
    return 0;
}

UINT TcExpression::GetSize()
{
    return 0;
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

bool TcGroupExpression::ProcessExpression(
    TDataArray<TrecPointer<TcExpression>>& expression,
    TDataArray<tc_int_op>& operators,
    TrecPointer<TcProcedureRunner> runner,
    TrecPointer<TEnvironment> environment,
    ReturnObject& ret,
    UINT& index,
    TcOperatorGroup& group,
    TrecPointer<TcOperator> theOp)
{
    if (index >= operators.Size())
    {
        ret.returnCode = ret.ERR_INTERNAL;
        ret.errorMessage.Set(L"Index Out of Bounds for Operator Access in Expression");
        return false;
    }

    if (!group.ContainsOperator(operators[index]))
        return false;

    if ((index + 1) >= expression.Size())
    {
        ret.returnCode = ret.ERR_UNEXPECTED_TOK;
        ret.errorMessage.Set(L"Index Out of Bounds for Expression Access in Group Expression");
        return false;
    }

    TrecPointer<TVariable> v1;
    TrecPointer<TVariable> v2;

    expression[index]->Express(runner, environment, ret);
    if (ret.returnCode)
        return false;
    v1 = ret.errorObject;
    if (operators[index] != tc_int_op::basic_exp && operators[index] != tc_int_op::point_exp)
    {
        expression[index + 1]->Express(runner, environment, ret);
        if (ret.returnCode)
            return false;
        v2 = ret.errorObject;
    }

    bool attemptAssign = false;
    switch (operators[index])
    {
    case tc_int_op::add_assign:
        attemptAssign = true;
    case tc_int_op::add:
        theOp->Add(v1, v2, ret);
        break;

    case tc_int_op::sub_assign:
        attemptAssign = true;
    case tc_int_op::sub:
        theOp->Sub(v1, v2, ret);
        break;

    case tc_int_op::mul_assign:
        attemptAssign = true;
    case tc_int_op::mult:
        theOp->Mul(v1, v2, ret);
        break;

    case tc_int_op::div_assign:
        attemptAssign = true;
    case tc_int_op::div:
        theOp->Div(v1, v2, ret);
        break;

    case tc_int_op::mod_assign:
        attemptAssign = true;
    case tc_int_op::mod:
        theOp->Mod(v1, v2, ret);
        break;

    case tc_int_op::pow_assign:
        attemptAssign = true;
    case tc_int_op::pow:
        theOp->Pow(v1, v2, ret);
        break;

    case tc_int_op::and_assign:
    case tc_int_op::or_assign:
    case tc_int_op::xor_assign:
        attemptAssign = true;
    case tc_int_op::and_l:
    case tc_int_op::or_l:
    case tc_int_op::xor_l:
        theOp->LogicalOp(v1, v2, operators[index], ret);
        break;
    case tc_int_op::b_and_assign:
    case tc_int_op::b_or_assign:
    case tc_int_op::b_xor_assign:
    case tc_int_op::b_left_assign:
    case tc_int_op::b_right_assign:
        attemptAssign = true;
    case tc_int_op::and_b:
    case tc_int_op::or_b:
    case tc_int_op::xor_b:
    case tc_int_op::left_b:
    case tc_int_op::right_b:
        theOp->BitwiseOp(v1, v2, operators[index], ret);
        break;

    case tc_int_op::eq:
    case tc_int_op::eq_t:
    case tc_int_op::gt:
    case tc_int_op::gte:
    case tc_int_op::lt:
    case tc_int_op::lte:
    case tc_int_op::not_e:
    case tc_int_op::not_e_t:
        theOp->EqualityCheck(v1, v2, operators[index], ret);
        break;

    case tc_int_op::basic_exp:
    case tc_int_op::point_exp:
        break;
    }

    if (ret.returnCode)
        return false;

    if (attemptAssign)
    {
        if (!expression[index]->CanAssign())
        {
            ret.returnCode = ret.ERR_UNSUPPORTED_OP;
            ret.errorMessage.Set(L"Attempt to Assign to unassignable Expression detected!");
            return false;
        }
        TrecPointer<TcAssignableExpression> assignExp = TrecPointerKey::ConvertPointer<TcExpression, TcAssignableExpression>(expression[index]);

        assignExp->Assign(runner, ret.errorObject, ret);
        if (ret.returnCode)
            return false;
    }

    return true;
}

TcGroupExpression::TcGroupExpression(TrecPointer<TStringVariable> operatorEngine)
{
    assert(operatorEngine.Get());
    this->operatorEngine = operatorEngine;
}

void TcGroupExpression::Express(TrecPointer<TcProcedureRunner> runner, TrecPointer<TEnvironment> environment, ReturnObject& ret)
{
    TrecPointer<TcOperatorGroupList> groupList = runner->GetOperatorGroupList();

    TrecPointer<TcOperator> theOps = TrecPointerKey::ConvertPointer<TObject, TcOperator>(environment->GetResource(operatorEngine->GetString()));

    if (!theOps.Get())
    {
        ret.returnCode = ret.ERR_BROKEN_REF;
        ret.errorMessage.Format(L"Operator Engine '%ws' Not Available!", operatorEngine->GetString().GetConstantBuffer().getBuffer());
        return;
    }

    if (!groupList.Get())
    {
        ret.returnCode = ret.ERR_BROKEN_REF;
        ret.errorMessage.Set(L"Information on Operator Precedence Not Provided!");
        return;
    }
    TcOperatorGroup group;

    auto expressionCopy = this->expressions;
    auto operatorsCopy = this->operators;

    for (UINT Rust = 0; groupList->GetGroup(group, Rust); Rust++)
    {
        bool isLeftToRight = group.IsLeftToRight();

        if (!operatorsCopy.Size())
            break;

        if (isLeftToRight)
        {
            for (UINT C = 0; C < operators.Size(); C++)
            {
                if (ProcessExpression(expressionCopy, operatorsCopy, runner, environment, ret, C, group, theOps))
                {
                    expressionCopy[C] = TrecPointerKey::ConvertPointer<TVariable, TcExpression>( TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TcValueExpression>(ret.errorObject));
                    expressionCopy.RemoveAt(C + 1);
                    operatorsCopy.RemoveAt(C--);
                }
            }
        }
        else
        {
            for (UINT C = operators.Size() -1; C < operators.Size(); C--)
            {
                if (ProcessExpression(expressionCopy, operatorsCopy, runner, environment, ret, C, group, theOps))
                {
                    expressionCopy[C] = TrecPointerKey::ConvertPointer<TVariable, TcExpression>( TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TcValueExpression>(ret.errorObject));
                    expressionCopy.RemoveAt(C + 1);
                    operatorsCopy.RemoveAt(C++);
                }
            }
        }
    }

    if (expressionCopy.Size() != 1)
    {
        ret.returnCode = ret.ERR_GENERIC_ERROR;
        ret.errorMessage.Set(L"Expression did not Equate to one variable upon Analysis!");
        ret.errorObject.Nullify();
    }

    expressionCopy[0]->Express(runner, environment, ret);
}

bool TcGroupExpression::NeedsResources()
{
    for (UINT Rust = 0; Rust < expressions.Size(); Rust++)
    {
        if (expressions[Rust]->NeedsResources())
            return true;
    }
    return false;
}

bool TcGroupExpression::Append(TrecPointer<TcExpression> value)
{
    if(!value.Get())
        return false;
    expressions.push_back(value);
    return true;
}

void TcGroupExpression::Append(tc_int_op op)
{
    operators.push_back(op);
}

TcValueExpression::TcValueExpression(TrecPointer<TVariable> value)
{
    this->value = value;
}

void TcValueExpression::Express(TrecPointer<TcProcedureRunner> runner, TrecPointer<TEnvironment> environment, ReturnObject& ret)
{
    ret.errorObject = value;
}

bool TcValueExpression::NeedsResources()
{
    return false;
}

bool TcValueExpression::Append(TrecPointer<TcExpression> value)
{
    return false;
}
