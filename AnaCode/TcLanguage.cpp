#include "TcLanguage.h"
#include <cassert>


// Json fields to help determine String types
TDataMap<str_recognition> jsonStringRecognition;

void PrepJsonStringRecognition()
{
	if (jsonStringRecognition.count())
		return;

	jsonStringRecognition.addEntry(L"StringLexSingleFinalPrim", str_recognition::str_single_final_prim);
	jsonStringRecognition.addEntry(L"StringLexSingleFinalSec", str_recognition::str_single_final_second);
	jsonStringRecognition.addEntry(L"StringLexSingleTemplatePrim", str_recognition::str_single_template_prim);
	jsonStringRecognition.addEntry(L"StringLexSingleTemplateSec", str_recognition::str_single_template_second);
	jsonStringRecognition.addEntry(L"StringLexMultiFinalPrim", str_recognition::str_multi_final_prim);
	jsonStringRecognition.addEntry(L"StringLexMultiFinalSec", str_recognition::str_multi_final_second);
	jsonStringRecognition.addEntry(L"StringLexMultiTemplatePrim", str_recognition::str_multi_template_prim);
	jsonStringRecognition.addEntry(L"StringLexMultiTemplateSec", str_recognition::str_multi_template_second);
}

TString TcLanguage::InitLexing()
{
	assert(languageDetails.Get());

	TString ret(L"Failed to Parse");
	UINT failures = 0;

	void PrepJsonStringRecognition();

	TDataEntry<str_recognition> strEntry;
	for (UINT Rust = 0; Rust < jsonStringRecognition.count() &&
		jsonStringRecognition.GetEntryAt(Rust, strEntry); Rust++)
	{
		TString baseCase(strEntry.key);
		TString startCase(baseCase + L"Start");
		TString endCase(baseCase + L"End");

		TrecPointer<TVariable> field;

		if (languageDetails->RetrieveField(baseCase, field) 
			&& field.Get())
		{
			if (field->GetVarType() == var_type::list)
			{
				auto listField = dynamic_cast<TArrayVariable*>(field.Get());
				TrecPointer<TVariable> field1;
				for (UINT C = 0; listField->GetValueAt(C, field1) && field1.Get(); C++)
				{
					TString strMark(dynamic_cast<TStringVariable*>(field1->ToString().Get())->GetString());
					stringRecognizers.push_back(StringRecognition(strEntry.object, strMark, strMark));
				}
			}
			else if(field->GetVarType() == var_type::string)
			{
				TString strMark(dynamic_cast<TStringVariable*>(field->ToString().Get())->GetString());
				stringRecognizers.push_back(StringRecognition(strEntry.object, strMark, strMark));
			}
			else
			{
				if (failures++) ret.AppendChar(L',');
				ret.AppendChar(L' ');
				ret.Append(baseCase);
				continue;
			}
		}
		else if (languageDetails->RetrieveField(startCase, field)
			&& field.Get())
		{
			TrecPointer<TVariable> field1;
			if (!languageDetails->RetrieveField(endCase, field1) || !field1.Get())
			{
				if(failures++) ret.AppendChar(L',');
				ret.AppendChar(L' ');
				ret.Append(endCase);
				continue;
			}

			if (field->GetVarType() == var_type::list && field1->GetVarType() == var_type::list &&
				dynamic_cast<TArrayVariable*>(field.Get())->GetSize() == dynamic_cast<TArrayVariable*>(field1.Get())->GetSize())
			{
				auto listField = dynamic_cast<TArrayVariable*>(field.Get());
				auto listField1 = dynamic_cast<TArrayVariable*>(field1.Get());
				TrecPointer<TVariable> field_, field1_;
				for (UINT C = 0; listField->GetValueAt(C, field_) && field_.Get() &&
					listField1->GetValueAt(C, field1_) && field1_.Get(); C++)
				{
					TString strMark(dynamic_cast<TStringVariable*>(field_->ToString().Get())->GetString());
					TString strMark1(dynamic_cast<TStringVariable*>(field1_->ToString().Get())->GetString());
					stringRecognizers.push_back(StringRecognition(strEntry.object, strMark, strMark1));
				}
			}
			else if (field->GetVarType() == var_type::string && field1->GetVarType() == var_type::string)
			{
				TString strMark(dynamic_cast<TStringVariable*>(field->ToString().Get())->GetString());
				TString strMark1(dynamic_cast<TStringVariable*>(field1->ToString().Get())->GetString());
				stringRecognizers.push_back(StringRecognition(strEntry.object, strMark, strMark1));
			}
			else
			{
				if (failures++) ret.AppendChar(L',');
				ret.AppendChar(L' ');
				ret.Append(startCase + L':' + endCase);
				continue;
			}
		}

	}

	TrecPointer<TVariable> field;

	if (languageDetails->RetrieveField(L"StringTemplateStart", field) && field.Get())
	{
		templateStart.Set(dynamic_cast<TStringVariable*>(field->ToString().Get())->GetString());
	}

	if (languageDetails->RetrieveField(L"StringTemplateEnd", field) && field.Get())
	{
		templateEnd.Set(dynamic_cast<TStringVariable*>(field->ToString().Get())->GetString());
	}

	if (languageDetails->RetrieveField(L"SingleCommentStart", field) && field.Get())
	{
		singlelineCommentStart.Set(dynamic_cast<TStringVariable*>(field->ToString().Get())->GetString());
	}

	if (languageDetails->RetrieveField(L"MultiCommentStart", field) && field.Get())
	{
		multiLineCommentStart.Set(dynamic_cast<TStringVariable*>(field->ToString().Get())->GetString());
	}

	if (languageDetails->RetrieveField(L"MultiCommentEnd", field) && field.Get())
	{
		multiLineCommentEnd.Set(dynamic_cast<TStringVariable*>(field->ToString().Get())->GetString());
	}

	if (languageDetails->RetrieveField(L"IdentifierStart", field) && field.Get())
	{
		identifierStart.Set(dynamic_cast<TStringVariable*>(field->ToString().Get())->GetString());
	}

	if (languageDetails->RetrieveField(L"IdentifierCharacter", field) && field.Get())
	{
		identifierCharacter.Set(dynamic_cast<TStringVariable*>(field->ToString().Get())->GetString());
	}

	if (languageDetails->RetrieveField(L"LexOperators", field) && field.Get() && field->GetVarType() == var_type::list)
	{
		auto listField = dynamic_cast<TArrayVariable*>(field.Get());
		for (UINT Rust = 0; listField->GetValueAt(Rust, field) && field.Get(); Rust++)
			operators.push_back(dynamic_cast<TStringVariable*>(field->ToString().Get())->GetString());
	}

	if (languageDetails->RetrieveField(L"NumberExpressions", field) && field.Get())
	{
		if (field->GetVarType() == var_type::list)
		{
			auto listField = dynamic_cast<TArrayVariable*>(field.Get());
			for (UINT Rust = 0; listField->GetValueAt(Rust, field) && field.Get(); Rust++)
			{
				auto pieces = dynamic_cast<TStringVariable*>(field->ToString().Get())->GetString().splitn(L":", 2);
				if (pieces->Size() != 2)
				{
					if (failures++) ret.AppendChar(L',');
					ret.AppendChar(L' ');
					ret.Append(L"NumberExpressions");
				}
				else
					numberRegExpression.addEntry(pieces->at(0).GetTrim(), pieces->at(1).GetTrim());
			}
		}
		else if (field->GetVarType() == var_type::string)
		{
			auto pieces = dynamic_cast<TStringVariable*>(field->ToString().Get())->GetString().splitn(L":", 2);
			if (pieces->Size() != 2)
			{
				if (failures++) ret.AppendChar(L',');
				ret.AppendChar(L' ');
				ret.Append(L"NumberExpressions");
			}
			else
				numberRegExpression.addEntry(pieces->at(0).GetTrim(), pieces->at(1).GetTrim());
		}
	}

	return L"";
}

bool TcLanguage::LexWasComment(TDataArray<Token>& tokens, TrecPointer<TStringVariable> code, UINT& loc, UINT& line, UINT& lineLoc, TDataArray<TcCompMessage>& messages)
{
	// Check for single line comment first
	TString& localCode = code->GetString();
	UINT curLine = line;
	UINT curLoc = loc;
	if (localCode.StartsAt(singlelineCommentStart, loc))
	{
		// We have found a single line comment
		UINT backslashCount = 0;
		UINT rCount = 0;
		bool leaveLoop = false;
		for (; !leaveLoop && loc < localCode.GetSize(); loc++, lineLoc++)
		{
			WCHAR ch = localCode[loc];
			switch (ch)
			{
			case L'\\':
				backslashCount++;
				rCount = 0;
				break;
			case L'\r':
				if (rCount) backslashCount = 0;
				rCount++;
				break;
			case L'\n':
				if (!(backslashCount % 2))
					leaveLoop = true;
				line++;
				lineLoc = 0;
			}
		}
		Token tok;
		tok.SubmitTokenType(token_type::tt_comment);
		tok.lineStart = curLine;
		tok.stringStart = curLoc;
		tok.tokenlength = loc - curLoc;
		tokens.push_back(tok);
		return true;
	}

	if (localCode.StartsAt(multiLineCommentStart, loc))
	{
		int endCommentLoc = localCode.Find(multiLineCommentEnd, loc + multiLineCommentStart.GetSize());

		if (endCommentLoc == -1)
		{
			TcCompMessage message;
			message.code.Set(L"Lex 01");
			message.start = loc;
			message.level = 1;
			message.message.Set(L"Unending Multi-line Comment!");
			loc = localCode.GetSize() + 1;
			messages.push_back(message);
			endCommentLoc = loc;
		}
		else loc = endCommentLoc + multiLineCommentEnd.GetSize();
		Token tok;
		tok.SubmitTokenType(token_type::tt_comment);
		tok.lineStart = line;
		tok.stringStart = curLoc;
		tok.tokenlength = loc - curLoc;
		tokens.push_back(tok);

		TString comment(localCode.SubString(curLoc, loc));
		line += comment.CountFinds(L'\n');

		endCommentLoc = localCode.FindLast(L'\n', loc);
		if (endCommentLoc == -1)
			endCommentLoc = 0;
		lineLoc = loc - endCommentLoc;
		return true;
	}
	return false;
}

bool TcLanguage::LexWasString(TDataArray<Token>& tokens, TrecPointer<TStringVariable> code, UINT& loc, UINT& line, UINT& lineLoc, TDataArray<TcCompMessage>& messages)
{
	for (UINT Rust = 0; Rust < stringRecognizers.Size(); Rust++)
	{
		if (code->GetString().StartsAt(stringRecognizers[Rust].startString, loc))
		{
			loc += stringRecognizers[Rust].startString.GetSize();
			lineLoc += stringRecognizers[Rust].startString.GetSize();

			switch (stringRecognizers[Rust].recognition)
			{
			case str_recognition::str_single_final_prim:
			case str_recognition::str_single_final_second:
				LexSingleLineFinalString(tokens, code, loc, line, lineLoc, messages, stringRecognizers[Rust]);
				break;

			}

			return true;
		}
	}
	return false;
}

bool TcLanguage::LexWasIdentifier(TDataArray<Token>& tokens, TrecPointer<TStringVariable> code, UINT& loc, UINT& line, UINT& lineLoc, TDataArray<TcCompMessage>& messages)
{
	WCHAR ch = code->GetString()[loc];
	if (identifierStart.Find(ch) != -1)
	{
		UINT curLoc = loc;
		for (; loc < code->GetString().GetSize(); loc++, lineLoc++)
		{
			if(identifierCharacter.Find(code->GetString()[loc] == -1))
				break;
		}
		Token tok;
		tok.SubmitTokenType(token_type::tt_identifier);
		tok.lineStart = line;
		tok.stringStart = curLoc;
		tok.tokenlength = loc - curLoc;
		tokens.push_back(tok);

		// Will be bumped back up in Calling function
		loc--;
		lineLoc--;

		return true;
	}
	return false;
}

bool TcLanguage::LexWasOperator(TDataArray<Token>& tokens, TrecPointer<TStringVariable> code, UINT& loc, UINT& line, UINT& lineLoc, TDataArray<TcCompMessage>& messages)
{
	for (UINT Rust = 0; Rust < operators.Size(); Rust++)
	{
		if (code->GetString().StartsAt(operators[Rust], Rust))
		{
			Token tok;
			tok.SubmitTokenType(token_type::tt_operator);
			tok.lineStart = line;
			tok.stringStart = loc;
			tok.tokenlength = operators[Rust].GetSize();
			tokens.push_back(tok);

			loc += tok.tokenlength - 1; // Will be bumped up in calling function
			lineLoc += tok.tokenlength - 1;
			return true;
		}
	}
	return false;
}

bool TcLanguage::LexWasNumber(TDataArray<Token>& tokens, TrecPointer<TStringVariable> code, UINT& loc, UINT& line, UINT& lineLoc, TDataArray<TcCompMessage>& messages)
{
	return false;
}

bool TcLanguage::LexSingleLineFinalString(TDataArray<Token>& tokens, TrecPointer<TStringVariable> code, UINT& loc, UINT& line, UINT& lineLoc, TDataArray<TcCompMessage>& messages, StringRecognition& recog)
{
	TString& localCode = code->GetString();

	UINT backslashCount = 0;
	UINT rCount = 0;
	bool leaveLoop = false;
	UINT curLoc = loc;
	UINT curLine = line;
	for (; !leaveLoop && loc < localCode.GetSize(); loc++, lineLoc++)
	{
		if (localCode.StartsAt(recog.endString, loc))
		{
			Token tok;
			tok.SubmitTokenType(recog.recognition == str_recognition::str_single_final_prim ? token_type::tt_single_string : token_type::tt_single_string_second);
			tok.stringStart = curLoc;
			tok.tokenlength = loc - curLoc;
			tok.lineStart = curLine;
			loc += recog.endString.GetSize() - 1;
			lineLoc += recog.endString.GetSize() - 1;
			tokens.push_back(tok);
			return true;
		}

		WCHAR ch = localCode[loc];
		switch (ch)
		{
		case L'\\':
			backslashCount++;
			rCount = 0;
			break;
		case L'\r':
			if (rCount) backslashCount = 0;
			rCount++;
			break;
		case L'\n':
			if (!(backslashCount % 2))
			{
				TcCompMessage message;
				message.code.Set(L"Lex 02");
				message.level = 3;
				message.message.Set("Unescaped newline detected in Single Line String!");
				message.start = curLoc;
				message.length = loc - curLoc;
				leaveLoop = true;
			}
			line++;
			lineLoc = 0;
		}
	}

	Token tok;
	tok.SubmitTokenType(recog.recognition == str_recognition::str_single_final_prim ? token_type::tt_single_string : token_type::tt_single_string_second);
	tok.stringStart = curLoc;
	tok.tokenlength = loc - curLoc - 1;
	tok.lineStart = curLine;

	loc--;
	lineLoc--;
	tokens.push_back(tok);

	return true;
}

TcLanguage::TcLanguage(TrecPointer<TJsonVariable> languageDetails)
{
	this->languageDetails = languageDetails;
	prepStage = 0;
}

TString TcLanguage::Init()
{
	if (prepStage)
		return L"";

	TString ret(InitLexing());
	if (ret.GetSize())
		return ret;
	prepStage++;
}

UCHAR TcLanguage::PerformLexScanning(TDataArray<Token>& tokens, TrecPointer<TStringVariable>& code, UINT& loc, UINT& line, UINT& lineLoc, TDataArray<TcCompMessage>& messages)
{
	for (; loc < code->GetSize(); loc++, lineLoc)
	{
		if (LexWasComment(tokens, code, loc, line, lineLoc, messages) ||
			LexWasString(tokens, code, loc, line, lineLoc, messages) ||
			LexWasIdentifier(tokens, code, loc, line, lineLoc, messages) ||
			LexWasOperator(tokens, code, loc, line, lineLoc, messages) ||
			LexWasNumber(tokens, code, loc, line, lineLoc, messages))
			continue;

		WCHAR ch = code->GetString()[loc];

		Token tok;
		tok.lineStart = line;
		tok.stringStart = loc;
		tok.tokenlength = 1;

		switch (ch)
		{
		case L'\n':
			tok.SubmitTokenType(token_type::tt_new_line);
			line++;
			lineLoc = 0;
			tokens.push_back(tok);
			break;
		case L'\t':
			tok.SubmitTokenType(token_type::tt_tab_space);
			tokens.push_back(tok);
			break;
		case L':':
			tok.SubmitTokenType(token_type::tt_colon);
			tokens.push_back(tok);
			break;
		case L';':
			tok.SubmitTokenType(token_type::tt_semicolon);
			tokens.push_back(tok);
			break;
		case L'(':
			tok.SubmitTokenType(token_type::tt_open_parenth);
			tokens.push_back(tok);
			break;
		case L')':
			tok.SubmitTokenType(token_type::tt_close_parenth);
			tokens.push_back(tok);
			break;
		case L'[':
			tok.SubmitTokenType(token_type::tt_open_square);
			tokens.push_back(tok);
			break;
		case L']':
			tok.SubmitTokenType(token_type::tt_close_square);
			tokens.push_back(tok);
			break;
		case L'{':
			tok.SubmitTokenType(token_type::tt_open_curly);
			tokens.push_back(tok);
			break;
		case L'}':
			tok.SubmitTokenType(token_type::tt_close_curly);
			tokens.push_back(tok);
			break;

		}

	}
	return 0;
}

StringRecognition::StringRecognition(str_recognition strRec, const TString& start, const TString& end)
{
	this->recognition = strRec;
	this->startString.Set(start);
	this->endString.Set(end);
}


token_type Token::GetTokenType()
{
	return static_cast<token_type>(tokenType);
}

void Token::SubmitTokenType(token_type type)
{
	this->tokenType = static_cast<UCHAR>(type);
}

TString Token::ToString(TrecPointer<TStringVariable> code)
{
	if (!code.Get())
		return TString();
	return code->GetString().SubString(stringStart, stringStart + tokenlength);
}
