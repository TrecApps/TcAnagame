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
		TrecPointer<TVariable> prop;
		TrecPointer<TJsonVariable> jProp;
		// To-Do: Flesh out Language Details
		languageDetails->RetrieveField(L"LexProps", prop);
		jProp = TrecPointerKey::ConvertPointer<TVariable, TJsonVariable>(prop);

		if (!jProp.Get())
			return L"Failed to Deduce Lex Properties!";

		TString ret(L"Failed to Parse");
		UINT failures = 0;

		if (!PrepParsing(statementseperator, "StatementSeparator", jProp))
		{
			if (failures++)
				ret.AppendChar(L',');
			ret.Append(" StatementSeparator");
		}
		if (!PrepParsing(singleLineComment, "CommentSingleLine", jProp))
		{
			if (failures++)
				ret.AppendChar(L',');
			ret.Append(" Comments (Single-Line)");
		}
		if (!PrepParsing(multiLineCommentStart, "MultilineCommentStart", jProp))
		{
			if (failures++)
				ret.AppendChar(L',');
			ret.Append(" Comments (Milti-line start)");
		}
		if (!PrepParsing(multiLineCommentEnd, "MultilineCommentEnd", jProp))
		{
			if (failures++)
				ret.AppendChar(L',');
			ret.Append(" Comments (Multi-Line End)");
		}
		if (!PrepParsing(singleString, "SingleLineString", jProp))
		{
			if (failures++)
				ret.AppendChar(L',');
			ret.Append(" String (Single-Line)");
		}
		if (!PrepParsing(multiString, "MultiLineString", jProp))
		{
			if (failures++)
				ret.AppendChar(L',');
			ret.Append(" String (Multi-Line)");
		}
		if (!PrepParsing(blockStart, "BlockStart", jProp))
		{
			if (failures++)
				ret.AppendChar(L',');
			ret.Append(" Block Start");
		}
		if (!PrepParsing(blockEnd, "BlockEnd", jProp))
		{
			if (failures++)
				ret.AppendChar(L',');
			ret.Append(" Block End");
		}
		if (!PrepParsing(flexString, "FlexString", jProp))
		{
			if (failures++)
				ret.AppendChar(L',');
			ret.Append(" FlexString");
		}
		if (!PrepParsing(flexStringBlock, "FlexStringBlock", jProp))
		{
			if (failures++)
				ret.AppendChar(L',');
			ret.Append(" Flex-String block");
		}

		if (failures)
			return ret;
		stage = 2;
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
