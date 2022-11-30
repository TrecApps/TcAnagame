#pragma once
#include <TString.h>
#include <TFile.h>
#include <TEnvironment.h>
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

using TcCompMessage = struct TcCompMessage {
    UCHAR level;
    TString code;
    TString message;
    UINT start;
    USHORT length;
};

class TcCompiler :
    public TVObject
{
protected:
    TrecPointerSoft<TEnvironment> environment;

    TrecPointer<TFileShell> file;
    TrecPointer<TStringVariable> text;

    TDataArray<TcLex> lexList;
    TDataArray<TcCompMessage> messages;

    UCHAR stage;

public:
    TcCompiler(
        TrecActivePointer<TFileShell> file,                 // The File that the Compiler will be Processing
        TrecActivePointer<TEnvironment> env,                // The Environment in which the Compiler operates under
        TrecActivePointer<TJsonVariable> languageDetails);  // Details of the language to Compile

    TString Init();         // Sets up the language attributes and opens the file for processing

    TString PerformLex();   // Run Lex Parsing on the Source String

    void OnSourceChange(TrecPointer<TStringVariable> updatedSource);  // Lets the Compiler know that the Text has been changed so that ReLexing can be done

    TrecPointer<TStringVariable> GetSourceString();

    virtual TString Compile() = 0;


};

