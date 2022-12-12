#include "TcLanguage.h"

void TcLanguage::ProcessComment(UINT& index, const TString& text, TDataArray<TcLex>& lexList, lex_mode mode)
{

}

void TcLanguage::ProcessString(UINT& index, const TString& text, const TString& end, TDataArray<TcLex>& lexList, lex_mode mode)
{
}

TcLanguage::lex_mode TcLanguage::GetNextMode(UINT index, const TString& text, UINT& begins)
{
	using LexModeIndex = struct LexModeIndex {
		lex_mode mode;
		UINT index;
	};

	TDataArray<LexModeIndex> retCollection;

	UINT size = text.GetSize();

	// Single Comment
	UINT searchIndex = size;
	for (UINT Rust = 0; Rust < singleLineComment.Size(); Rust++)
	{
		int found = text.Find(singleLineComment[Rust], index);
		if (found != -1 && found < searchIndex)
			searchIndex = found;
	}
	retCollection.push_back({ lex_mode::single_comment, searchIndex });

	// Multi-Comment
	searchIndex = size;
	for (UINT Rust = 0; Rust < multiLineCommentStart.Size(); Rust++)
	{
		int found = text.Find(multiLineCommentStart[Rust], index);
		if (found != -1 && found < searchIndex)
			searchIndex = found;
	}
	retCollection.push_back({ lex_mode::multi_comment, searchIndex });

	// Single String
	searchIndex = size;
	for (UINT Rust = 0; Rust < singleString.Size(); Rust++)
	{
		int found = text.Find(singleString[Rust], index);
		if (found != -1 && found < searchIndex)
			searchIndex = found;
	}
	retCollection.push_back({ lex_mode::single_string, searchIndex });

	// Multi-String
	searchIndex = size;
	for (UINT Rust = 0; Rust < multiString.Size(); Rust++)
	{
		int found = text.Find(multiString[Rust], index);
		if (found != -1 && found < searchIndex)
			searchIndex = found;
	}
	retCollection.push_back({ lex_mode::multi_string, searchIndex });


	int retIndex = -1;

	for (UINT Rust = 0; Rust < retCollection.Size(); Rust++)
	{
		if (retCollection[Rust].index < size)
		{
			retIndex = Rust;
			size = retCollection[Rust].index;
		}
	}

	if (retIndex == -1)
		return lex_mode::normal;

	begins = retCollection[retIndex].index;

	return retCollection[retIndex].mode;
}

TcLanguage::TcLanguage(TrecPointer<TJsonVariable> languageDetails)
{
	this->languageDetails = languageDetails;
	this->stage = 0;
}

TString TcLanguage::Init()
{
	if(stage)
		return TString();
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
	stage = 1;
}

bool TcLanguage::PrepParsing(TDataArray<TString>& parsList, const TString& propKey, TrecPointer<TJsonVariable> propSource)
{
	parsList.RemoveAll();

	TrecPointer<TVariable> property;
	if (!propSource->RetrieveField(propKey, property))
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

TString TcLanguage::PerformLex(TrecPointer<TStringVariable> var, TDataArray<TcLex>& lexList)
{
	TString& actString(var->GetString());

	lexList.RemoveAll();

	lex_mode lexMode = lex_mode::normal;


	for (UINT Rust = 0; Rust < actString.GetSize(); Rust++)
	{
		switch (actString.GetAt(Rust))
		{
		case L'\\':
			lexList.push_back({ tc_lex_type::line_break, Rust, 1 });
			break;
		case L'(':
			lexList.push_back({ tc_lex_type::open_parenth, Rust, 1 });
			break;
		case L'[':
			lexList.push_back({ tc_lex_type::open_square, Rust, 1 });
			break;
		case L')':
			lexList.push_back({ tc_lex_type::close_parenth, Rust, 1 });
			break;
		case L']':
			lexList.push_back({ tc_lex_type::close_square, Rust, 1 });
			break;
		case L'\r':
			if (Rust + 1 < actString.GetSize() && actString.GetAt(Rust + 1) == L'\n')
			{
				lexList.push_back({ tc_lex_type::new_line, Rust++, 2});
				continue;
			}
		case L'\n':
			lexList.push_back({ tc_lex_type::new_line, Rust++, 1 });
			break;

		}
	}
}

