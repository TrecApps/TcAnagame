#pragma once

#include <TFile.h>
#include <TEnvironment.h>
#include <TContainerVariable.h>
#include "TcLanguage.h"




class TcCompiler :
    public TVObject
{
protected:
    TrecPointerSoft<TEnvironment> environment;

    TrecPointer<TFileShell> file;
    TrecPointer<TStringVariable> text;

    TDataArray<TcCompMessage> messages;
    TrecPointer<TcLanguage> language;

    UCHAR stage;


    bool PrepParsing(TDataArray<TString>& parsList, const TString& propKey, TrecPointer<TJsonVariable> propSource);

public:
    TcCompiler(
        TrecActivePointer<TFileShell> file,                 // The File that the Compiler will be Processing
        TrecActivePointer<TEnvironment> env,                // The Environment in which the Compiler operates under
        TrecActivePointer<TcLanguage> languageDetails);  // Details of the language to Compile

    TString Init();         // Sets up the language attributes and opens the file for processing

    TString PerformLex();   // Run Lex Parsing on the Source String

    void OnSourceChange(TrecPointer<TStringVariable> updatedSource);  // Lets the Compiler know that the Text has been changed so that ReLexing can be done

    TrecPointer<TStringVariable> GetSourceString();

    virtual TString Compile() = 0;


};

