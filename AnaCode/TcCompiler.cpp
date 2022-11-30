#include "TcCompiler.h"

TcCompiler::TcCompiler(TrecActivePointer<TFileShell> file, TrecActivePointer<TEnvironment> env, TrecActivePointer<TJsonVariable> languageDetails)
{
	stage = 0;
	this->file = TrecPointerKey::ActiveToTrec<>(file);
	if (this->file->IsDirectory())
		throw 3;

	environment = TrecPointerKey::SoftFromTrec<>(TrecPointerKey::ActiveToTrec<>(env));
	this->languageDetails = TrecPointerKey::ActiveToTrec<>(languageDetails);
}

TString TcCompiler::Init()
{
	if (!stage)
	{
		TFile reader(file, TFile::t_file_open_existing | TFile::t_file_read);
		if (!reader.IsOpen())
			return L"Failed to Open File!";
		TString contents;
		TString contentSlice;

		while (reader.ReadStringLine(contentSlice, 500))
			contents.Append(contentSlice);

		text = TrecPointerKey::ConvertPointer<TVariable, TStringVariable>(
			TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(contents));
		stage++;
	}

	if (stage == 1)
	{
		// To-Do: Flesh out Language Details
	}

	return TString();
}

TString TcCompiler::PerformLex()
{
	return TString();
}

void TcCompiler::OnSourceChange(TrecPointer<TStringVariable> updatedSource)
{
	if (updatedSource.Get())
	{
		text = updatedSource;
		PerformLex();
	}
}

TrecPointer<TStringVariable> TcCompiler::GetSourceString()
{
	if (!text.Get())
		return TrecPointer<TStringVariable>();
	return TrecPointerKey::ConvertPointer<TVariable, TStringVariable>(
		TrecPointerKey::GetNewTrecPointerAlt<TVariable, TStringVariable>(text->GetString()));
}
