#include "TcCompiler.h"

bool TcCompiler::PrepParsing(TDataArray<TString>& parsList, const TString& propKey, TrecPointer<TJsonVariable> propSource)
{
	parsList.RemoveAll();

	TrecPointer<TVariable> property;
	if(!propSource->RetrieveField(propKey, property))
	return false;

	TrecPointer<TArrayVariable> arrayProp = TrecPointerKey::ConvertPointer<TVariable, TArrayVariable>(property);
	if (!arrayProp.Get())
		return false;

	for (UINT Rust = 0; Rust < arrayProp->GetValueAt(Rust, property); Rust++)
	{
		TrecPointer<TStringVariable> strProp = TrecPointerKey::ConvertPointer<TVariable, TStringVariable>(property);
		if (strProp.Get())
			parsList.push_back(strProp->GetString());
	}

	return true;
}

TcCompiler::TcCompiler(TrecActivePointer<TFileShell> file, TrecActivePointer<TEnvironment> env, TrecActivePointer<TcLanguage> languageDetails)
{
	stage = 0;
	this->file = file.GetTrecPointer();
	if (this->file->IsDirectory())
		throw 3;
	auto tEnv = env.GetTrecPointer();
	environment = TrecPointerKey::SoftFromTrec<>(tEnv);
	this->language = languageDetails.GetTrecPointer();
}

TString TcCompiler::Init(TrecPointer<TStringVariable> text)
{
	if (!stage)
	{
		if (!text.Get())
		{
			TFile reader(file, TFile::t_file_open_existing | TFile::t_file_read);
			if (!reader.IsOpen())
				return L"Failed to Open File!";
			TString contents;
			TString contentSlice;

			while (reader.ReadStringLine(contentSlice, 500))
				contents.Append(contentSlice);

			this->text = TrecPointerKey::ConvertPointer<TVariable, TStringVariable>(
				TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(contents));
		}
		else
			this->text = text;
		stage++;
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

TString TcCompiler::Compile()
{
	return TString();
}
