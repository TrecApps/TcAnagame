#include "TcRunner.h"

VariableHolder::VariableHolder()
{
	isArray = false;
	mutLevel = 0;
	nullable = true;
}

VariableHolder::VariableHolder(const VariableHolder& copy) : type(copy.type), value(copy.value)
{
	isArray = copy.isArray;
	mutLevel = copy.mutLevel;
	nullable = copy.nullable;
}

VariableHolder& VariableHolder::operator=(const VariableHolder& copy)
{
	isArray = copy.isArray;
	mutLevel = copy.mutLevel;
	value = copy.value;
	nullable = copy.nullable;
	type.Set(type);
	return *this;
}

VariableHolder::~VariableHolder()
{
}

ReturnObject::ReturnObject()
{
	returnCode = nextCount = 0;
	mode = return_mode::rm_regular;
}

ReturnObject::ReturnObject(const ReturnObject& copy)
{
	errorMessage.Set(copy.errorMessage);
	errorObject = copy.errorObject;
	errorObject2 = copy.errorObject2;
	returnCode = copy.returnCode;
	stackTrace = copy.stackTrace;
	mode = copy.mode;
	nextCount = copy.nextCount;
}

ReturnObject& ReturnObject::operator=(const ReturnObject& copy)
{
	errorMessage.Set(copy.errorMessage);
	errorObject = copy.errorObject;
	errorObject2 = copy.errorObject2;
	returnCode = copy.returnCode;
	stackTrace = copy.stackTrace;
	mode = copy.mode;
	nextCount = copy.nextCount;
	return *this;
}

var_type TcRunner::GetVarType()
{
	return var_type::runner;
}

void TcRunner::GetParameterSpecs(TDataArray<VariableHolder>& specs)
{
	specs = variableSpecs;
}

void TcRunner::SetParameterSpecs(const TDataArray<VariableHolder>& specs)
{
	variableSpecs = specs;
}

void TcRunner::SetParameters(TrecPointer<TVariable>& params, ReturnObject& result)
{
	
}

void TcRunner::SetTerminate(ULONG64 termination)
{
	this->termination = termination;
}

void TcRunner::Terminate()
{
	doTerminate = true;
}

void TcRunner::Run(ReturnObject& ret)
{
	if (doAsync)
	{
		TrecPointer<TcRunner> thisRunner = TrecPointerKey::ConvertPointer<TVariable, TcRunner>(
			Clone()
			);
		thisRunner->doAsync = false;

		ret.errorObject = TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TcAsyncVariable>(thisRunner);
	}
	else
		RunDetails(ret);
}

TrecPointer<TVariable> TcRunner::ToString()
{
	return TrecPointer<TVariable>();
}

TrecPointer<TVariable> TcRunner::ToString(TrecPointer<TVariable> detail)
{
	return ToString();
}

UINT TcRunner::Get4Value()
{
	return 0;
}

UINT TcRunner::GetSize() {
	return 0;
}

var_type TcAsyncVariable::GetVarType()
{
	return var_type::async;
}

TrecPointer<TVariable> TcAsyncVariable::Clone()
{
	return TrecPointerKey::TrecFromSoft<>(vSelf);
}

TrecPointer<TVariable> TcAsyncVariable::ToString()
{
	return TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(L"[Async Variable]");
}

TrecPointer<TVariable> TcAsyncVariable::ToString(TrecPointer<TVariable> detail)
{
	return ToString();
}

UINT TcAsyncVariable::Get4Value()
{
	return 0;
}

UINT TcAsyncVariable::GetSize()
{
	return 0;
}

TcAsyncVariable::TcAsyncVariable(TrecPointer<TcRunner> mainRunner)
{
	this->mainFunction = mainRunner;
	this->doTerminate = false;
	this->process = 0;

	this->threadReference = nullptr;
	
	TcInitLock(&thread);
}

TcAsyncVariable::~TcAsyncVariable()
{
	if (threadReference)
	{
		doTerminate = true;
		threadReference->join();

		delete threadReference;
		threadReference = nullptr;
	}
}

// This method is expected to be run 
void TcAsyncVariable::Invoke_(TrecPointer<TcAsyncVariable> thisVar)
{
	thisVar->process = 0;
	ReturnObject ret;
	if (thisVar->mainFunction.Get())
		thisVar->mainFunction->Run(ret);

	for (UINT Rust = 0; !thisVar->doTerminate && Rust < thisVar->response.Size(); Rust++)
	{
		if (!ret.returnCode)
		{
			auto accepted = thisVar->response[Rust].accepted;
			if (accepted.Get())
			{
				accepted->SetParameters(ret.errorObject, ret);
				accepted->Run(ret);
			}
		}
		else
		{
			auto rejected = thisVar->response[Rust].rejected;
			if (rejected.Get())
			{
				rejected->SetParameters(ret.errorObject, ret);
				rejected->Run(ret);
				break;
			}
		}
	}

	if (!thisVar->doTerminate && thisVar->finalCall.Get())
		thisVar->finalCall->Run(ret);

	thisVar->varHolder.value = ret.errorObject;

	thisVar->process = ret.returnCode ? -1 : 1;

	
}

void TcAsyncVariable::SetExpectedType(const TString& type)
{
	varHolder.type.Set(type);
}

void TcAsyncVariable::Invoke()
{
	TrecPointer<TcAsyncVariable> thisVar = TrecPointerKey::ConvertPointer<TVariable, TcAsyncVariable>(
		TrecPointerKey::TrecFromSoft<>(vSelf)
		);
	threadReference = new std::thread(Invoke_, thisVar);
}

TrecPointer<TVariable> TcAsyncVariable::GetResult() const
{
	return process ? varHolder.value : TrecPointer<TVariable>();
}

signed char TcAsyncVariable::GetProgess() const
{
	return process;
}

void TcAsyncVariable::AppendResponse(TrecPointer<TcRunner> success, TrecPointer<TcRunner> rejected)
{
	if (success.Get() || rejected.Get())
		response.push_back({ success, rejected });
}

void TcAsyncVariable::SetFinalResponse(TrecPointer<TcRunner> finallyRunner)
{
	finalCall = finallyRunner;
}
