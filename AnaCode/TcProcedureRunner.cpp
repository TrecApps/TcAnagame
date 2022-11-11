#include "TcProcedureRunner.h"

TcVariableHolder::TcVariableHolder()
{
	mut = true;
}

TcVariableHolder::TcVariableHolder(const TcVariableHolder& copy)
{
	mut = copy.mut;
	type.Set(copy.type);
	value = copy.value;
}

TcVariableHolder::TcVariableHolder(bool mut, const TString& type, TrecPointer<TVariable> value)
{
	this->mut = mut;
	this->type = type;
	this->value = value;
}

TcProcedureRunner::TcProcedureRunner(TrecPointer<TEnvironment> environment, TrecPointer<TcProcedureRunner> parentRunner)
{
	this->environment = environment;
	this->parentRunner = TrecPointerKey::SoftFromTrec<>(parentRunner);
}

void TcProcedureRunner::SetParamNames(TDataArray<TString>& paramNames)
{
	this->paramNames = paramNames;
}

void TcProcedureRunner::SetParamTypes(TDataArray<TString>& paramTypes)
{
	this->paramTypes = paramTypes;
}
