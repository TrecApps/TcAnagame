#pragma once
#include <TString.h>
#include <TContainerVariable.h>

using tc_lex_type = enum class tc_lex_type
{
    // String management
    single_line_string_bounds,  // Boundaries of a single line string
    multi_line_string_bounds,   // Boundaries of a multi-line string
    string_reg,                 // Regular String (runners treat this as the final draft
    string_flex,                // Flexible String (runners will copy and update the copy based off of expressions found)
    string_exp_start,           // Marker in a flexbile string signaling the start of an expression
    string_exp_end,             // Marker in a Flexible string signaling the end

    // Comment management
    single_line_comment,        // Start of a Single Line Comment
    multi_line_comment_start,   // Start of a Multiline comment
    multi_line_comment_end,     // End of a multiline Comment
    multi_line_comment,         // Body of a multiline comment

    // organization
    line_break,                 // Signal that the next line should be treated as part of "this" line
    new_line,                   // New Line Token
    block_start,                // A new Block of code began
    block_end,                  // The current Block of Code has come to an end
    statement_break,            // Signals the end of a statement

    // Regular Lex
    op,                         // Represents an Operator
    number,                     // Represents a number
    identifier,                 // Identifies some data to reference
    key_word,                   // Key Word
    open_parenth,               // Open Parenthesis
    close_parenth,              // Close Parenthesis
    open_square,                // Open Square Brackets
    close_square               // Close Square Brackets 
};

using TcLex = struct TcLex {
    tc_lex_type type;
    UINT start;
    USHORT length;
};

class TcLanguage :
    public TObject
{
protected:

    
    TrecPointer<TJsonVariable> languageDetails;

    TDataArray<TString> statementseperator, // How to recognize when a Statement ends
        singleLineComment,                  // Token for recognizing the start of a single line comment
        multiLineCommentStart,              // Tokens for recognizing start of  multiline comment
        multiLineCommentEnd,                // Tokens for recognizing End of a multiline comment
        singleString,                       // Tokens for recognizing a single line String
        multiString,                        // Tokens for recognizing a multiline String
        blockStart,                         // Tokens for recognizing the start of a block (if this is blank, indentation is assumed)
        blockEnd,                           // Tokens for recognizing the end of a block
        flexString,                   // Strings that, if they start the statement, signal that the statement should be one line
        flexStringBlock,                    // Signal that an expression in a flex string has been encountered
        keyWords;
    TString varStart;
    TString numStart;
    TString varSection;
    TString numSection;
    TString operatorChars;
    
    UCHAR stage;

    using lex_mode = enum class lex_mode
    {
        normal,
        single_comment,
        multi_comment,
        single_string,
        multi_string
    };

    void ProcessComment(UINT& index, const TString& text, TDataArray<TcLex>& lexList, lex_mode mode, UINT size);
    void ProcessString(UINT& index, const TString& text, const TString& end, TDataArray<TcLex>& lexList, lex_mode mode);

    void ProcessNumber(UINT& index, const TString&, TDataArray<TcLex>& lexList);
    void ProcessVariable(UINT& index, const TString&, TDataArray<TcLex>& lexList);
    void ProcessOperator(UINT& index, const TString&, TDataArray<TcLex>& lexList);

    lex_mode GetNextMode(UINT index, const TString& text, UINT& begins, UINT& size);


public: 
    TcLanguage(TrecPointer<TJsonVariable> languageDetails);
    TString Init();
    bool PrepParsing(TDataArray<TString>& parsList, const TString& propKey, TrecPointer<TJsonVariable> propSource);

    bool PrepParsing(TrecPointer<TVariable> prop, TString& result);


    TString PerformLex(TrecPointer<TStringVariable> var, TDataArray<TcLex>& lexList);   // Run Lex Parsing on the Source String
};

