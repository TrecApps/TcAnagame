#include "TcLanguage.h"

using LineBreaker = struct LineBreaker
{
	TString str;
	int foundAt;
	bool isSlash;
};

void TcLanguage::ProcessComment(UINT& index, const TString& text, TDataArray<TcLex>& lexList, lex_mode mode, UINT size)
{
	
	if (mode == lex_mode::single_comment)
	{
		TcLex lex{ tc_lex_type::single_line_comment, index, size };
		int slash = -1;
		int newline = -1;

		LineBreaker breakers[] = {
			{L"\\\n", 0, true},
			{L"\\\r\n", 0, true},
			{L"\\\r", 0, true},
			{L"\n", 0, false},
			{L"\r\n", 0, false},
			{L"\r", 0, false},
		};

		bool commentContinues = false;

		do
		{
			for (UINT Rust = 0; Rust < ARRAYSIZE(breakers); Rust++)
			{
				breakers[Rust].foundAt = text.Find(breakers[Rust].str, index);
			}

			int targetBreakers = -1;

			for (int Rust = 0; Rust < ARRAYSIZE(breakers); Rust++)
			{
				if (breakers[Rust].foundAt != -1)
				{
					if (targetBreakers == -1 || (breakers[Rust].foundAt < breakers[targetBreakers].foundAt))
						targetBreakers = Rust;
				}
			}

			if (targetBreakers == -1)
			{
				lex.length = text.GetSize() - lex.start - 1;
				lexList.push_back(lex);
				index = text.GetSize();
				return;
			}

			commentContinues = false;

			// Now make sure that if we have a slash, then ensure it is an odd Number
			if (breakers[targetBreakers].isSlash)
			{
				UINT count = 1;
				for (UINT Rust = breakers[targetBreakers].foundAt - 1; Rust > index && Rust < text.GetSize(); Rust--)
				{
					WCHAR ch = text.GetAt(Rust);
					if (ch == L'\\')
						count++;
					else
						break;
				}

				if (count % 2)
					commentContinues = true;
			}

			lex.length = breakers[targetBreakers].foundAt - lex.start;
			lexList.push_back(lex);

			index = breakers[targetBreakers].foundAt + breakers[targetBreakers].str.GetSize() - 1;

			lexList.push_back({ 
				tc_lex_type::new_line,
				static_cast<UINT>(breakers[targetBreakers].foundAt),
				static_cast<USHORT>(breakers[targetBreakers].str.GetSize()) 
			});

			lex.start = index + 1;

		} while (commentContinues);
	}
	else if (mode == lex_mode::multi_comment)
	{
		TcLex lex{ tc_lex_type::multi_line_comment_start, index, size };
		lexList.push_back(lex);

		index += size;

		UINT end = text.GetSize();
		UINT endIndex = 0;
		for (UINT Rust = 0; Rust < multiLineCommentEnd.Size(); Rust++)
		{
			int potentilEnd = text.Find(multiLineCommentEnd[Rust], index);
			if (potentilEnd != -1 && potentilEnd < end)
			{
				end = potentilEnd;
				endIndex = multiLineCommentEnd[Rust].GetSize();
			}
		}

		bool commentContinue = true;

		LineBreaker breakers[] = {
			{L"\n", 0, false},
			{L"\r\n", 0, false},
			{L"\r", 0, false},
		};

		do
		{
			for (UINT Rust = 0; Rust < ARRAYSIZE(breakers); Rust++)
			{
				breakers[Rust].foundAt = text.Find(breakers[Rust].str, index);
			}

			int targetBreakers = -1;

			for (int Rust = 0; Rust < ARRAYSIZE(breakers); Rust++)
			{
				if (breakers[Rust].foundAt != -1)
				{
					if (targetBreakers == -1 || (breakers[Rust].foundAt < breakers[targetBreakers].foundAt))
						targetBreakers = Rust;
				}
			}
			commentContinue = false;

			if (targetBreakers != -1)
			{
				lexList.push_back({ tc_lex_type::multi_line_comment, index, static_cast<USHORT>(breakers[targetBreakers].foundAt - index) });

				lexList.push_back({
				tc_lex_type::new_line,
				static_cast<UINT>(breakers[targetBreakers].foundAt),
				static_cast<USHORT>(breakers[targetBreakers].str.GetSize())
					});

				index = breakers[targetBreakers].foundAt + breakers[targetBreakers].str.GetSize();
				commentContinue = true;
			}
		} while (commentContinue);

		lexList.push_back({ tc_lex_type::multi_line_comment, index, static_cast<USHORT>(end - index) });

		index = end;
		lexList.push_back({ tc_lex_type::multi_line_comment_end, index, static_cast<USHORT>(endIndex)});

		index += endIndex;
	}
}

void TcLanguage::ProcessString(UINT& index, const TString& text, const TString& end, TDataArray<TcLex>& lexList, lex_mode mode)
{
	// Check to see if we are dealing with a flex String
	bool isFlex = false;
	for (UINT Rust = 0; !isFlex && Rust < flexString.Size(); Rust++)
	{
		if (!end.Compare(flexString[Rust]))
			isFlex = true;
	}

	TrecPointer<TDataArray<TString>> flexMarkers = this->flexStringBlock.Size() ? flexStringBlock[0].splitn(L'*', 2) : TrecPointer<TDataArray<TString>>();

	// If We don't have flex markers, then we are not doing flex
	if (!flexMarkers.Get() || flexMarkers->Size() != 2)
		isFlex = false;

	TDataArray<TString> seek;
	seek.push_back(text);
	seek.push_back(L"\r\n");
	seek.push_back(L"\n");
	seek.push_back(L"\r");

	UINT seekIndex = 0;
	int strEnd = -1;

		seekSingleEnd:
		strEnd = text.FindOneOf(seek, seekIndex, index, false);
		if (strEnd == -1)
			return;

		if (isFlex)
		{
			// To-Do: Handle Flex Case
		}
		else if (seekIndex > 0)
		{
			if (mode == lex_mode::single_string) return;

			lexList.push_back({ tc_lex_type::string_reg, index, static_cast<USHORT>(strEnd - index) });
			lexList.push_back({ tc_lex_type::new_line, static_cast<UINT>(strEnd), static_cast<USHORT>(seek[seekIndex].GetSize())});
			index = strEnd + seek[seekIndex].GetSize();
			goto seekSingleEnd;
		}


		lexList.push_back({ tc_lex_type::string_reg, index, static_cast<USHORT>(strEnd - index) });
		lexList.push_back({
			mode == lex_mode::single_string ? tc_lex_type::single_line_string_bounds : tc_lex_type::multi_line_string_bounds,
			static_cast<UINT>(strEnd),
			static_cast<USHORT>(text.GetSize())
			});
}

void TcLanguage::ProcessNumber(UINT& index, const TString& text, TDataArray<TcLex>& lexList)
{
	for (UINT Rust = index; Rust < text.GetSize(); Rust++)
	{
		if (numSection.Find(text.GetAt(Rust)) == -1)
		{
			lexList.push_back({ tc_lex_type::number, index, static_cast<USHORT>(Rust - index) });
			index = Rust - 1;
			return;
		}
	}
	lexList.push_back({ tc_lex_type::number, index, static_cast<USHORT>(text.GetSize() - index) });
	index = text.GetSize() -1;
}

void TcLanguage::ProcessVariable(UINT& index, const TString& text, TDataArray<TcLex>& lexList)
{
	UINT Rust = index;
	for (; Rust < text.GetSize(); Rust++)
	{
		if (varSection.Find(text.GetAt(Rust)) == -1)
		{
			lexList.push_back({ tc_lex_type::number, index, static_cast<USHORT>(Rust - index) });
			break;
		}
	}

	TString varSlice(text.SubString(index, Rust));
	
	bool found = false;
	for (UINT C = 0; !found && C < keyWords.Size(); C++)
	{
		if (keyWords[C].Compare(varSlice))
		{
			lexList.push_back({ tc_lex_type::key_word, index, static_cast<USHORT>(Rust - index) });
			found = true;
		}
	}
	if(!found)
		lexList.push_back({ tc_lex_type::identifier, index, static_cast<USHORT>(Rust - index) });
	
	index = Rust - 1;
}

void TcLanguage::ProcessOperator(UINT& index, const TString& text, TDataArray<TcLex>& lexList)
{
	for (UINT Rust = index; Rust < text.GetSize(); Rust++)
	{
		if (operatorChars.Find(text.GetAt(Rust)) == -1)
		{
			lexList.push_back({ tc_lex_type::op, index, static_cast<USHORT>(Rust - index) });
			index = Rust - 1;
			return;
		}
	}
	lexList.push_back({ tc_lex_type::op, index, static_cast<USHORT>(text.GetSize() - index) });
	index = text.GetSize() - 1;
}

TcLanguage::lex_mode TcLanguage::GetNextMode(UINT index, const TString& text, UINT& begins, UINT& lexSize)
{
	using LexModeIndex = struct LexModeIndex {
		lex_mode mode;
		UINT index;
		UINT size;
	};

	TDataArray<LexModeIndex> retCollection;

	UINT size = text.GetSize();

	// Single Comment
	UINT searchIndex = size;
	UINT searchSize = 0;
	for (UINT Rust = 0; Rust < singleLineComment.Size(); Rust++)
	{
		int found = text.Find(singleLineComment[Rust], index);
		if (found != -1 && found < searchIndex)
		{
			searchIndex = found;
			searchSize = singleLineComment[Rust].GetSize();
		}
	}
	retCollection.push_back({ lex_mode::single_comment, searchIndex, searchSize });

	// Multi-Comment
	searchIndex = size;
	searchSize = 0;
	for (UINT Rust = 0; Rust < multiLineCommentStart.Size(); Rust++)
	{
		int found = text.Find(multiLineCommentStart[Rust], index);
		if (found != -1 && found < searchIndex)
		{
			searchIndex = found;
			searchSize = singleLineComment[Rust].GetSize();
		}
	}
	retCollection.push_back({ lex_mode::multi_comment, searchIndex, searchSize });

	// Single String
	searchIndex = size;
	searchSize = 0;
	for (UINT Rust = 0; Rust < singleString.Size(); Rust++)
	{
		int found = text.Find(singleString[Rust], index);
		if (found != -1 && found < searchIndex)
		{
			searchIndex = found;
			searchSize = singleLineComment[Rust].GetSize();
		}
	}
	retCollection.push_back({ lex_mode::single_string, searchIndex, searchSize });

	// Multi-String
	searchIndex = size;
	searchSize = 0;
	for (UINT Rust = 0; Rust < multiString.Size(); Rust++)
	{
		int found = text.Find(multiString[Rust], index);
		if (found != -1 && found < searchIndex)
		{
			searchIndex = found;
			searchSize = singleLineComment[Rust].GetSize();
		}
	}
	retCollection.push_back({ lex_mode::multi_string, searchIndex, searchSize });


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
	lexSize = retCollection[retIndex].size;

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
	if (!PrepParsing(keyWords, "KeyWords", jProp))
	{
		if (failures++)
			ret.AppendChar(L',');
		ret.Append(" Key Words");
	}

	if (!jProp->RetrieveField(L"VarStart", prop) || !PrepParsing(prop, varStart))
	{
		if (failures++)
			ret.AppendChar(L',');
		ret.Append(" Start of Variables");
	}
	if (!jProp->RetrieveField(L"numStart", prop) || !PrepParsing(prop, numStart))
	{
		if (failures++)
			ret.AppendChar(L',');
		ret.Append(" Start of Number");
	}
	if (!jProp->RetrieveField(L"varSection", prop) || !PrepParsing(prop, varSection))
	{
		if (failures++)
			ret.AppendChar(L',');
		ret.Append(" Characters in Variable");
	}
	if (!jProp->RetrieveField(L"numSection", prop) || !PrepParsing(prop, numSection))
	{
		if (failures++)
			ret.AppendChar(L',');
		ret.Append(" Characters in Numbers");
	}
	if (!jProp->RetrieveField(L"operatorChars", prop) || !PrepParsing(prop, operatorChars))
	{
		if (failures++)
			ret.AppendChar(L',');
		ret.Append(" Operator Characters");
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

bool TcLanguage::PrepParsing(TrecPointer<TVariable> prop, TString& result)
{
	TrecPointer<TStringVariable> strProp = TrecPointerKey::ConvertPointer<TVariable, TStringVariable>(prop);
	if(!strProp.Get())
		return false;
	result.Set(strProp->GetString());
	return true;
}

TString TcLanguage::PerformLex(TrecPointer<TStringVariable> var, TDataArray<TcLex>& lexList)
{
	TString& actString(var->GetString());

	lexList.RemoveAll();
	UINT beginMode = 0;
	UINT sizeMode = 0;
	lex_mode lexMode = GetNextMode(0, actString, beginMode, sizeMode);


	for (UINT Rust = 0; Rust < actString.GetSize(); Rust++)
	{
		TString quote;
		// First Handle Strings and Comments
		if (lexMode != lex_mode::normal && Rust == beginMode)
		{
			switch (lexMode)
			{
			case lex_mode::multi_comment:
			case lex_mode::single_comment:
				ProcessComment(Rust, actString, lexList, lexMode, sizeMode);
				break;
			case lex_mode::single_string:
				for (UINT C = 0; !quote.GetSize() && C < singleString.Size(); C++)
				{
					if (actString.Find(singleString[C], Rust) == Rust)
					{
						quote.Set(singleString[C]);
					}
				}
			case lex_mode::multi_string:
				for (UINT C = 0; !quote.GetSize() && C < multiString.Size(); C++)
				{
					if (actString.Find(multiString[C], Rust) == Rust)
					{
						quote.Set(multiString[C]);
					}
				}

				ProcessString(Rust, actString, quote, lexList, lexMode);
			}

			lexMode = GetNextMode(Rust, actString, beginMode, sizeMode);
		}

		WCHAR ch = actString.GetAt(Rust);

		switch (ch)
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
		default:

			if (operatorChars.Find(ch) != -1)
				ProcessOperator(Rust, actString, lexList);
			if (numStart.Find(ch) != -1)
				ProcessNumber(Rust, actString, lexList);
			if (varStart.Find(ch) != -1)
				ProcessVariable(Rust, actString, lexList);
		}
	}
}

