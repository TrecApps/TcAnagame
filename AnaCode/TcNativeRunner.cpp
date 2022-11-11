#include "TcNativeRunner.h"

TcNativeRunner::TcNativeRunner(NativeFunction f, TrecPointer<TEnvironment> environment, TrecPointer<TcProcedureRunner> parentRunner)
	:TcProcedureRunner(environment, parentRunner)
{
	this->function = f;
}

void TcNativeRunner::SetIntialVariables(TDataArray<TrecPointer<TVariable>>& params)
{
	initialVariables = params;
}

void TcNativeRunner::Run(ReturnObject& ret)
{
	function(this->initialVariables, environment, ret);
}
