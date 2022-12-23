#include "TcObjectRunner.h"

TcObjectRunner::TcObjectRunner(TrecActivePointer<TcObjectProcedure> proc, TrecPointer<TEnvironment> environment, TrecPointer<TcProcedureRunner> parentRunner)
	:TcProcedureRunner(environment, parentRunner)
{
	procedure = proc.GetTrecPointer();

	statementHandlers.push_back(&TcObjectRunner::ProcessRegular);

	statementHandlers.push_back(&TcObjectRunner::ProcessBlock);
	statementHandlers.push_back(&TcObjectRunner::ProcessIf);
	statementHandlers.push_back(&TcObjectRunner::ProcessNIf);
	statementHandlers.push_back(&TcObjectRunner::ProcessWhile);
	statementHandlers.push_back(&TcObjectRunner::ProcessUntil);
	statementHandlers.push_back(&TcObjectRunner::ProcessDoWhile);
	statementHandlers.push_back(&TcObjectRunner::ProcessFor1);
	statementHandlers.push_back(&TcObjectRunner::ProcessFor3);

	statementHandlers.push_back(&TcObjectRunner::ProcessSwitch);
	statementHandlers.push_back(&TcObjectRunner::ProcessCase);
	statementHandlers.push_back(&TcObjectRunner::ProcessSwitchDefault);

	statementHandlers.push_back(&TcObjectRunner::ProcessDeclareNew);
	statementHandlers.push_back(&TcObjectRunner::ProcessDeclareNewConst);
	statementHandlers.push_back(&TcObjectRunner::ProcessDeclareNewHoist);

	statementHandlers.push_back(&TcObjectRunner::ProcessTry);
	statementHandlers.push_back(&TcObjectRunner::ProcessCatch);
	statementHandlers.push_back(&TcObjectRunner::ProcessFinally);
	statementHandlers.push_back(&TcObjectRunner::ProcessThrow);

	statementHandlers.push_back(&TcObjectRunner::ProcessReturn);
	statementHandlers.push_back(&TcObjectRunner::ProcessBreak);
	statementHandlers.push_back(&TcObjectRunner::ProcessContinue);
	statementHandlers.push_back(&TcObjectRunner::ProcessYeild);
	statementHandlers.push_back(&TcObjectRunner::ProcessGoTo);
	statementHandlers.push_back(&TcObjectRunner::ProcessGoToTarget);
}

TrecPointer<TVariable> TcObjectRunner::Clone()
{
	return TrecPointer<TVariable>();
}

void TcObjectRunner::SetIntialVariables(TDataArray<TrecPointer<TVariable>>& params)
{
}

void TcObjectRunner::Run(ReturnObject& ret)
{
	if (!theOps.Get())
	{
		ret.returnCode == ret.ERR_BROKEN_REF;
		ret.errorMessage.Set(L"Operator Object Not Set");
		return;
	}

	while (this->statementCounter < this->procedure->GetSize())
	{
		Resume(ret);

		if (!ret.returnCode && ret.mode == return_mode::rm_regular)
			continue;
		break;
	}
}

void TcObjectRunner::Init()
{
}

void TcObjectRunner::Resume(ReturnObject& ret)
{
	auto& statement = procedure->GetStatement(statementCounter);

	for (UINT Rust = 0; Rust < this->nextStack; Rust++)
	{
		if (statement.next.Get())
			statement = *statement.next.Get();
		else
		{
			nextStack = 0;
			statementCounter++;
			return;
		}
	}
	(this->*statementHandlers[static_cast<UINT>(statement.statementType)])(ret, statement);
}

void TcObjectRunner::ProcessRegular(ReturnObject& ret, TcStatement& statement)
{
}

void TcObjectRunner::ProcessIf(ReturnObject& ret, TcStatement& statement)
{
	if (!statement.primeExpression.Get())
	{
		ret.returnCode = ret.ERR_UNEXPECTED_TOK;
		ret.errorMessage.Set(L"Malformed If-Statement: Exprected an Expression of some sorts!");
		return;
	}
	bool worked = false;
	bool isTrue = false;

	statement.primeExpression->Express(GetProcedureRunner(), environment, ret);
	if (ret.returnCode)
		return;



	theOps->IsTruthful(ret.errorObject, worked, isTrue, ret);

	if (worked && isTrue)
	{
		GenerateBlockRunner(statement)->Run(ret);
	}
	else if (worked)
	{
		nextStack++;
		return;
	}

	if (!ret.returnCode)
	{
		statementCounter++;
	}

}

void TcObjectRunner::ProcessBlock(ReturnObject& ret, TcStatement& statement)
{
	GenerateBlockRunner(statement)->Run(ret);
}

void TcObjectRunner::ProcessNIf(ReturnObject& ret, TcStatement& statement)
{
	if (!statement.primeExpression.Get())
	{
		ret.returnCode = ret.ERR_UNEXPECTED_TOK;
		ret.errorMessage.Set(L"Malformed If-Statement: Exprected an Expression of some sorts!");
		return;
	}
	bool worked = false;
	bool isTrue = false;

	statement.primeExpression->Express(GetProcedureRunner(), environment, ret);
	if (ret.returnCode)
		return;



	theOps->IsTruthful(ret.errorObject, worked, isTrue, ret);

	if (worked && !isTrue)
	{
		GenerateBlockRunner(statement)->Run(ret);
	}
	else if (worked)
	{
		nextStack++;
		return;
	}

	if (!ret.returnCode)
	{
		statementCounter++;
	}
}

bool TcObjectRunner::doBlockReturn(ReturnObject& ret, TcStatement& statement)
{
	if (ret.errorObject.Get() && statement.name.GetSize())
	{
		auto cur = ret.errorObject;
		theOps->EqualityCheck(
			TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(statement.name),
			ret.errorObject,
			tc_int_op::eq,
			ret);

		if (ret.returnCode)
		{
			return true;
		}

		bool worked = false;
		bool isTrue = false;

		theOps->IsTruthful(ret.errorObject, worked, isTrue, ret);

		if (!worked)
			return true;
		if (isTrue)
			ret.mode = return_mode::rm_regular;
		else
		{
			ret.errorObject = cur;
			return true;
		}
	}
	return false;
}

void TcObjectRunner::ProcessWhile(ReturnObject& ret, TcStatement& statement)
{

	if (!statement.primeExpression.Get())
	{
		ret.returnCode = ret.ERR_UNEXPECTED_TOK;
		ret.errorMessage.Set(L"Malformed If-Statement: Exprected an Expression of some sorts!");
		return;
	}
	bool worked = false;
	bool isTrue = false;

	statement.primeExpression->Express(GetProcedureRunner(), environment, ret);
	if (ret.returnCode)
		return;
	theOps->IsTruthful(ret.errorObject, worked, isTrue, ret);

	while (worked && isTrue)
	{
		GenerateBlockRunner(statement)->Run(ret);

		if (!ret.returnCode)
			return;

		bool doBreak = false;
		switch (ret.mode)
		{
		case return_mode::rm_break:
			doBreak = true;
		case return_mode::rm_continue:
			if (doBreak || doBlockReturn(ret, statement))
				return;
			break;

		case return_mode::rm_return:
		case return_mode::rm_yield:
			return;
		}

		statement.primeExpression->Express(GetProcedureRunner(), environment, ret);
		if (ret.returnCode)
			return;
		theOps->IsTruthful(ret.errorObject, worked, isTrue, ret);
	}
	statementCounter++;
}

void TcObjectRunner::ProcessUntil(ReturnObject& ret, TcStatement& statement)
{
	if (!statement.primeExpression.Get())
	{
		ret.returnCode = ret.ERR_UNEXPECTED_TOK;
		ret.errorMessage.Set(L"Malformed If-Statement: Exprected an Expression of some sorts!");
		return;
	}
	bool worked = false;
	bool isTrue = false;

	statement.primeExpression->Express(GetProcedureRunner(), environment, ret);
	if (ret.returnCode)
		return;
	theOps->IsTruthful(ret.errorObject, worked, isTrue, ret);

	while (worked && !isTrue)
	{
		GenerateBlockRunner(statement)->Run(ret);

		if (!ret.returnCode)
			return;

		bool doBreak = false;
		switch (ret.mode)
		{
		case return_mode::rm_break:
			doBreak = true;
		case return_mode::rm_continue:
			if (doBreak || doBlockReturn(ret, statement))
				return;
			break;

		case return_mode::rm_return:
		case return_mode::rm_yield:
			return;
		}

		statement.primeExpression->Express(GetProcedureRunner(), environment, ret);
		if (ret.returnCode)
			return;
		theOps->IsTruthful(ret.errorObject, worked, isTrue, ret);
	}
	statementCounter++;
}

void TcObjectRunner::ProcessDoWhile(ReturnObject& ret, TcStatement& statement)
{
	if (!statement.primeExpression.Get())
	{
		ret.returnCode = ret.ERR_UNEXPECTED_TOK;
		ret.errorMessage.Set(L"Malformed If-Statement: Exprected an Expression of some sorts!");
		return;
	}
	bool worked = true;
	bool isTrue = true;

	while (worked && isTrue)
	{
		GenerateBlockRunner(statement)->Run(ret);

		if (!ret.returnCode)
			return;

		bool doBreak = false;
		switch (ret.mode)
		{
		case return_mode::rm_break:
			doBreak = true;
		case return_mode::rm_continue:
			if (doBreak || doBlockReturn(ret, statement))
				return;
			break;

		case return_mode::rm_return:
		case return_mode::rm_yield:
			return;
		}

		statement.primeExpression->Express(GetProcedureRunner(), environment, ret);
		if (ret.returnCode)
			return;
		theOps->IsTruthful(ret.errorObject, worked, isTrue, ret);
	}
	statementCounter++;
}

void TcObjectRunner::ProcessFor1(ReturnObject& ret, TcStatement& statement)
{
}

void TcObjectRunner::ProcessFor3(ReturnObject& ret, TcStatement& statement)
{
	//if(statement.primeExpression.Get())

}

void TcObjectRunner::ProcessSwitch(ReturnObject& ret, TcStatement& statement)
{
}

void TcObjectRunner::ProcessCase(ReturnObject& ret, TcStatement& statement)
{
}

void TcObjectRunner::ProcessSwitchDefault(ReturnObject& ret, TcStatement& statement)
{
}

void TcObjectRunner::ProcessDeclareNew(ReturnObject& ret, TcStatement& statement)
{
}

void TcObjectRunner::ProcessDeclareNewConst(ReturnObject& ret, TcStatement& statement)
{
}

void TcObjectRunner::ProcessDeclareNewHoist(ReturnObject& ret, TcStatement& statement)
{
}

void TcObjectRunner::ProcessTry(ReturnObject& ret, TcStatement& statement)
{
	ProcessBlock(ret, statement);

	if (ret.returnCode)
		ProcessCatch(ret, statement);
	else ProcessFinally(ret, statement);
}

void TcObjectRunner::ProcessCatch(ReturnObject& ret, TcStatement& statement)
{
	while (statement.statementType != statement_type::st_catch)
	{
		if (statement.next.Get())
			statement = *statement.next.Get();
		else
			return;
	}

	if (statement.statementType == statement_type::st_catch)
	{
		TString eName;
		TrecPointer<TVariable> exception = ret.errorObject;
		try
		{
			statement.primeExpression->Express(GetProcedureRunner(), environment, ret);
			TrecPointer<TStringVariable> veName = TrecPointerKey::ConvertPointer<TVariable, TStringVariable>(ret.errorObject);
			eName.Set(veName->GetString());
		}
		catch (...) {
			ret.errorMessage.Append(L"!!! Failed to properly handle Excetpion in Catch Block!");
			return;
		}

		ret.returnCode = 0;

		auto processor = this->GenerateBlockRunner(statement);
		processor->variables.addEntry(eName, TcVariableHolder(true, L"", exception));
		processor->Run(ret);
	}

	ProcessFinally(ret, statement);
}

void TcObjectRunner::ProcessFinally(ReturnObject& ret, TcStatement& statement)
{
	while (statement.statementType != statement_type::st_finally)
	{
		if (statement.next.Get())
			statement = *statement.next.Get();
		else
		{
			this->statementCounter++;
			return;
		}
	}

	ProcessBlock(ret, statement);
	statementCounter++;
}

void TcObjectRunner::ProcessThrow(ReturnObject& ret, TcStatement& statement)
{
	if (statement.primeExpression.Get())
		statement.primeExpression->Express(GetProcedureRunner(), environment, ret);

	ret.returnCode = ret.ERR_THROWN;
	statementCounter++;
}

void TcObjectRunner::ProcessReturn(ReturnObject& ret, TcStatement& statement)
{
	ret.errorObject.Nullify();
	if (statement.primeExpression.Get())
	{
		statement.primeExpression->Express(GetProcedureRunner(), environment, ret);
	}
	if (!ret.returnCode)
		ret.mode = return_mode::rm_return;
}

void TcObjectRunner::ProcessBreak(ReturnObject& ret, TcStatement& statement)
{
	ret.errorObject.Nullify();
	if (statement.primeExpression.Get())
	{
		statement.primeExpression->Express(GetProcedureRunner(), environment, ret);
	}
	if (!ret.returnCode)
		ret.mode = return_mode::rm_break;
}

void TcObjectRunner::ProcessContinue(ReturnObject& ret, TcStatement& statement)
{
	ret.errorObject.Nullify();
	if (statement.primeExpression.Get())
	{
		statement.primeExpression->Express(GetProcedureRunner(), environment, ret);
	}
	if (!ret.returnCode)
		ret.mode = return_mode::rm_continue;
}

void TcObjectRunner::ProcessYeild(ReturnObject& ret, TcStatement& statement)
{
	ret.errorObject.Nullify();
	if (statement.primeExpression.Get())
	{
		statement.primeExpression->Express(GetProcedureRunner(), environment, ret);
	}
	if (!ret.returnCode)
	{
		ret.mode = return_mode::rm_yield;
		this->statementCounter++;
	}
}

void TcObjectRunner::ProcessGoTo(ReturnObject& ret, TcStatement& statement)
{
}

void TcObjectRunner::ProcessGoToTarget(ReturnObject& ret, TcStatement& statement)
{
}

TrecPointer<TcProcedureRunner> TcObjectRunner::GetProcedureRunner()
{
	return TrecPointerKey::ConvertPointer<TVariable, TcProcedureRunner>( 
		TrecPointerKey::TrecFromSoft<>(this->vSelf));
}

TrecPointer<TcObjectRunner> TcObjectRunner::GenerateBlockRunner(TcStatement& statement)
{
	auto firstParam = statement.block;
	if (!firstParam.Get())
		firstParam = TrecPointerKey::ConvertPointer<TVariable, TcObjectProcedure>(TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TcObjectProcedure>());
	TrecPointer<TcObjectRunner> ret = TrecPointerKey::ConvertPointer<TVariable, TcObjectRunner>(
		TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TcObjectRunner>(
			TrecActivePointer<TcObjectProcedure>(firstParam),
			environment,
			GetProcedureRunner()
			)
		);

	ret->theOps = this->theOps;
	return ret;
}

void TcObjectRunner::RunBlock(ReturnObject& ret)
{
}
