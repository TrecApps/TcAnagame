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

void TcRunner::Run(ReturnObject& ret)
{
	if (doAsync)
	{

	}
	else
		RunDetails(ret);
}
