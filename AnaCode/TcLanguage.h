#pragma once
#include <TString.h>
#include <TContainerVariable.h>

using str_recognition = enum class str_recognition {
    str_single_final_prim,
    str_single_final_second,
    str_single_template_prim,
    str_single_template_second,
    str_multi_final_prim,
    str_multi_final_second,
    str_multi_template_prim,
    str_multi_template_second,
};

class StringRecognition
{
public:
    StringRecognition() = default;
    StringRecognition(const StringRecognition& copy) = default;
    StringRecognition(str_recognition strRec, const TString& start, const TString& end);

    str_recognition recognition;
    TString startString, endString;
};

using TcCompMessage = struct TcCompMessage {
    UCHAR level;
    TString code;
    TString message;
    UINT start;
    USHORT length;
};

using token_type = enum class token_type
{
    tt_comment,     // comments, which compilers/interpretors ignore
    tt_new_line,    // signifies a new line, not always used
    tt_tab_space,   // Useful for languages like python

    // Common Charaters in languages
    tt_semicolon,
    tt_colon,
    tt_open_curly,
    tt_close_curly,
    tt_open_parenth,
    tt_close_parenth,
    tt_open_square,
    tt_close_square,

    // common features
    tt_operator,
    tt_identifier,
    tt_number,
    tt_single_string,           // Basic String type for
    tt_multi_string,
    tt_single_string_second,    // Secondary String type (like C++'s L"" for wide-strings)
    tt_multi_string_second,
    tt_template_mark_start,     // used to mark the boundaries of code within strings (like JavaScript's `${ ... }` )
    tt_template_mark_end,
    tt_keywords,                // If we encountered a keyword
};



class Token
{
public:
    USHORT lineStart;   // the line in the String where the token starts
    USHORT stringStart; // the character in the string where the token starts
    UCHAR tokenType;    // The type of token
    UCHAR extraInfo;    // Extra information about this token (value meaning varies by token type)
    USHORT tokenlength; // the length of the token

    Token() = default;
    Token(const Token& copy) = default;

    token_type GetTokenType();
    void SubmitTokenType(token_type type);

    TString ToString(TrecPointer<TStringVariable>);
};


class TcLanguage :
    public TObject
{
protected:
    // Raw Information
    UCHAR prepStage;
    TrecPointer<TJsonVariable> languageDetails;

    // Lexing Data

    TString InitLexing();

    TDataArray<StringRecognition> stringRecognizers;
    TString templateStart;
    TString templateEnd;

    TString singlelineCommentStart;
    TString multiLineCommentStart;
    TString multiLineCommentEnd;

    TString identifierStart;
    TString identifierCharacter;

    TDataMap<TString> numberRegExpression;
    TDataArray<TString> operators;

    // Lexing Helpers
    bool LexWasComment(TDataArray<Token>& tokens, TrecPointer<TStringVariable> code, UINT& loc, UINT& line, UINT& lineLoc, TDataArray<TcCompMessage>& messages);
    bool LexWasString(TDataArray<Token>& tokens, TrecPointer<TStringVariable> code, UINT& loc, UINT& line, UINT& lineLoc, TDataArray<TcCompMessage>& messages);
    bool LexWasIdentifier(TDataArray<Token>& tokens, TrecPointer<TStringVariable> code, UINT& loc, UINT& line, UINT& lineLoc, TDataArray<TcCompMessage>& messages);
    bool LexWasOperator(TDataArray<Token>& tokens, TrecPointer<TStringVariable> code, UINT& loc, UINT& line, UINT& lineLoc, TDataArray<TcCompMessage>& messages);
    bool LexWasNumber(TDataArray<Token>& tokens, TrecPointer<TStringVariable> code, UINT& loc, UINT& line, UINT& lineLoc, TDataArray<TcCompMessage>& messages);
    //bool LexWasString(TDataArray<Token>& tokens, TrecPointer<TStringVariable> code, UINT& loc, UINT& line, UINT& lineLoc, TDataArray<TcCompMessage>& messages);

    // String Lexing helpers
    bool LexSingleLineFinalString(TDataArray<Token>& tokens, TrecPointer<TStringVariable> code, UINT& loc, UINT& line, UINT& lineLoc, TDataArray<TcCompMessage>& messages, StringRecognition& recog);

public: 
    TcLanguage(TrecPointer<TJsonVariable> languageDetails);
    TString Init();

    UCHAR PerformLexScanning(TDataArray<Token>& tokens, TrecPointer<TStringVariable>& code, UINT& loc, UINT& line, UINT& lineLoc, TDataArray<TcCompMessage>& messages);

};

