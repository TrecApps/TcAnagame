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

void TcProcedureRunner::SetMethodObject(TrecPointer<TVariable> methodObject)
{
	object = methodObject;
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

bool TcProcedureRunner::GetVariable(const TString& name, TcVariableHolder& value)
{
	bool ret = variables.retrieveEntry(name, value);
	if (!ret)
	{
		auto parent = TrecPointerKey::TrecFromSoft<>(parentRunner);

		if (parent.Get())
			ret = parent->GetVariable(name, value);
		else if (environment.Get())
		{
			value.value = environment->GetProperty(name);
			ret = value.value.Get() != nullptr;
			if (ret)
			{
				value.mut = false;
				value.type.Empty();
			}
		}
	}
	return ret;
}

bool TcProcedureRunner::UpdateVariable(const TString& name, const TString& type, TrecPointer<TVariable> value, bool appendLocal)
{
	TcVariableHolder holder;
	bool ret = variables.retrieveEntry(name, holder);
	if (!ret)
	{
		auto parent = TrecPointerKey::TrecFromSoft<>(parentRunner);
		if (parent.Get())
			ret = parent->UpdateVariable(name, type, value, false);

		if (ret || !appendLocal)
			return ret;
	}
	else if (!holder.mut)
	{
		// Can't update
		return false;
	}

	// To-Do: Type Checking

	// End To-Do
	holder.type.Set(type);
	holder.value = value;
	variables.setEntry(name, holder);


	return true;
}
