#include "TcCompiler.h"

TcCompiler::TcCompiler(TrecActivePointer<TFileShell> file, TrecActivePointer<TEnvironment> env, TrecActivePointer<TJsonVariable> languageDetails)
{
}

TString TcCompiler::Init()
{
	return TString();
}

TString TcCompiler::PerformLex()
{
	return TString();
}

void TcCompiler::OnSourceChange(TrecPointer<TStringVariable> updatedSource)
{
}

TrecPointer<TStringVariable> TcCompiler::GetSourceString()
{
	return TrecPointer<TStringVariable>();
}
