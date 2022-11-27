#include "TcObjectRunner.h"

TcObjectRunner::TcObjectRunner(TrecActivePointer<TcObjectProcedure> proc, TrecPointer<TEnvironment> environment, TrecPointer<TcProcedureRunner> parentRunner)
	:TcProcedureRunner(environment, parentRunner)
{
	procedure = TrecPointerKey::ActiveToTrec<>(proc);

	statementHandlers.push_back(&TcObjectRunner::ProcessRegular);

	statementHandlers.push_back(&TcObjectRunner::ProcessIf);
	statementHandlers.push_back(&TcObjectRunner::ProcessIf);
	statementHandlers.push_back(&TcObjectRunner::ProcessNIf);
	statementHandlers.push_back(&TcObjectRunner::ProcessWhile);
	statementHandlers.push_back(&TcObjectRunner::ProcessUntil);
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

void TcObjectRunner::SetIntialVariables(TDataArray<TrecPointer<TVariable>>& params)
{
}

void TcObjectRunner::Run(ReturnObject& ret)
{
}

void TcObjectRunner::Init()
{
}

void TcObjectRunner::Resume(ReturnObject& ret)
{
}

void TcObjectRunner::ProcessRegular(ReturnObject& ret)
{
}

void TcObjectRunner::ProcessIf(ReturnObject& ret)
{
}

void TcObjectRunner::ProcessNIf(ReturnObject& ret)
{
}

void TcObjectRunner::ProcessWhile(ReturnObject& ret)
{
}

void TcObjectRunner::ProcessUntil(ReturnObject& ret)
{
}

void TcObjectRunner::ProcessDoWhile(ReturnObject& ret)
{
}

void TcObjectRunner::ProcessFor1(ReturnObject& ret)
{
}

void TcObjectRunner::ProcessFor3(ReturnObject& ret)
{
}

void TcObjectRunner::ProcessSwitch(ReturnObject& ret)
{
}

void TcObjectRunner::ProcessCase(ReturnObject& ret)
{
}

void TcObjectRunner::ProcessSwitchDefault(ReturnObject& ret)
{
}

void TcObjectRunner::ProcessDeclareNew(ReturnObject& ret)
{
}

void TcObjectRunner::ProcessDeclareNewConst(ReturnObject& ret)
{
}

void TcObjectRunner::ProcessDeclareNewHoist(ReturnObject& ret)
{
}

void TcObjectRunner::ProcessTry(ReturnObject& ret)
{
}

void TcObjectRunner::ProcessCatch(ReturnObject& ret)
{
}

void TcObjectRunner::ProcessFinally(ReturnObject& ret)
{
}

void TcObjectRunner::ProcessThrow(ReturnObject& ret)
{
}

void TcObjectRunner::ProcessReturn(ReturnObject& ret)
{
}

void TcObjectRunner::ProcessBreak(ReturnObject& ret)
{
}

void TcObjectRunner::ProcessContinue(ReturnObject& ret)
{
}

void TcObjectRunner::ProcessYeild(ReturnObject& ret)
{
}

void TcObjectRunner::ProcessGoTo(ReturnObject& ret)
{
}

void TcObjectRunner::ProcessGoToTarget(ReturnObject& ret)
{
}

void TcObjectRunner::RunBlock(ReturnObject& ret)
{
}
