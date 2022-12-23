#include "TFormatReaderJson.h"
#include "TContainerVariable.h"
#include "TString.h"
#include "TPrimitiveVariable.h"

TString TFormatReaderJson::Read()
{
    file.Open(fileShell, TFile::t_file_open_existing | TFile::t_file_read);
    if (!file.IsOpen())
        return L"Failed to Open TML file!";

    TString lineStr;
    UINT line = 0;

    file.ReadString(lineStr, L"{", 1);

    lineStr.Trim();
    if (lineStr.GetSize() != 1)
        return L"Unexpected Characters found before initial '{' token!";

    TString ret;

    variable = this->ProcessObject(ret);


    return ret;
}

TString TFormatReaderJson::Write(TrecPointer<TVariable> data, write_mode mode)
{
    if (!fileShell.Get())
        return L"No File Provided!";

    TFile file(fileShell, TFile::t_file_open_always | TFile::t_file_write | TFile::t_file_truncate_existing);

    if (!file.IsOpen())
        return L"Failed to open file!";



    return TString();
}

TrecPointer<TVariable> TFormatReaderJson::GetData()
{
    return variable;
}

TFormatReaderJson::TFormatReaderJson(TrecPointer<TFileShell> file)
{
    this->fileShell = file;
}

TrecPointer<TVariable> TFormatReaderJson::ProcessArray(TString& worked)
{
    TString readable;

    TrecPointer<TArrayVariable> ret = TrecPointerKey::ConvertPointer<TVariable, TArrayVariable>(TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TArrayVariable>());

    do
    {
        file.ReadString(readable, L"[{,}]", 0b00000111); // We Can actually expect a String at this point, so make sure terminating character is out of quotes and not backslashed

        readable.Trim();
        if (!readable.Compare(L',') || !readable.Compare(L']'))
            continue;
        TrecPointer<TVariable> var;
        if (readable.EndsWith(L'['))
            var = ProcessArray(worked);
        else if (readable.EndsWith(L'{'))
            var = ProcessObject(worked);
        else if (readable.EndsWith(L',') || readable.EndsWith(L']'))
        {
            bool bWorked = true;
            var = ProccessValue(readable.SubString(0, readable.GetSize() - 1), bWorked);
            if (!bWorked)
                worked.Format(L"Unexpected Tokens detected when assessing attribute %d, Expected String/number/boolean/null!", ret->GetSize() + 1);
        }
        else if (!readable.EndsWith(L'}'))
        {
            worked.Format(L"Unexpected End of File detected when assessing attribute %d", ret->GetSize() + 1);
        }
        if (worked.GetSize())
            return TrecPointer<TVariable>();

        // We have a Key/Value, add it to returned object
        ret->Push(var);
    } while (!readable.EndsWith(L']'));

    return TrecPointerKey::ConvertPointer<TArrayVariable, TVariable>(ret);
}

TrecPointer<TVariable> TFormatReaderJson::ProcessObject(TString& worked)
{
    TString readable;
    UINT read;

    TrecPointer<TJsonVariable> ret = TrecPointerKey::ConvertPointer<TVariable, TJsonVariable>(TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TJsonVariable>());
    UINT loopCount = 1;
    do
    {
        read = file.ReadString(readable, L"}\"", 1);
        if (readable.StartsWith(L','))
            readable.Delete(0);
        readable.Trim();
        if (readable.GetSize() > 1)
            worked.Format(L"Unexpected Tokens detected when assessing attribute %d", loopCount);
        else if (!readable.GetSize())
            worked.Format(L"Unexpected End of File detected when assessing attribute %d", loopCount);

        if (worked.GetSize())
            return TrecPointer<TVariable>();
        if (readable[0] == L'\"')
        {
            TString name;
            if (!ProcessName(name))
            {
                worked.Format(L"Invalid name detected for attribute %d", loopCount);
                return TrecPointer<TVariable>();
            }

            read = file.ReadString(readable, L":", 1); // Just looking for one character, no need to stor it

            if (!read)
                worked.Format(L"Unexpected End of File detected when assessing attribute %d", loopCount);
            readable.Trim();
            if (readable.GetSize() != 1)
                worked.Format(L"Unexpected Tokens detected when assessing attribute %d, expected ':'!", loopCount);

            if (worked.GetSize())
                return TrecPointer<TVariable>();

            file.ReadString(readable, L"[{,}", 0b00000111);

            TrecPointer<TVariable> var;
            if (readable.EndsWith(L'['))
                var = ProcessArray(worked);
            /*   else if (readable.EndsWith(L'\'') || readable.EndsWith(L'\"'))
               {
                   WCHAR quoteChar = readable[readable.GetSize() - 1];
                   read = file.ReadString(readable, quoteChar, 0b00000101);
                   if(!readable.EndsWith(quoteChar))
                       worked.Format("")
               }*/
            else if (readable.EndsWith(L'{'))
                var = ProcessObject(worked);
            else if (readable.EndsWith(L',') || readable.EndsWith(L'}'))
            {
                bool bWorked = true;
                var = ProccessValue(readable.SubString(0, readable.GetSize() - 1), bWorked);
                if (!bWorked)
                    worked.Format(L"Unexpected Tokens detected when assessing attribute %d, Expected String/number/boolean/null!", loopCount);
            }
            else if (!readable.EndsWith(L'}'))
            {
                worked.Format(L"Unexpected End of File detected when assessing attribute %d", loopCount);
            }
            if (worked.GetSize())
                return TrecPointer<TVariable>();

            // We have a Key/Value, add it to returned object
            ret->SetField(name, var);
        }
        loopCount++;
    } while (!readable.EndsWith(L'}') && read);
    return TrecPointerKey::ConvertPointer<TJsonVariable, TVariable>(ret);
}

bool TFormatReaderJson::ProcessName(TString& name)
{
    file.ReadString(name, L"\"", 1);

    if (!name.EndsWith(L"\"") || name.CountFinds(L'\''))
        return false;

    name.Delete(name.GetSize() - 1);
    name.Trim();
    return true;
}

TrecPointer<TVariable> TFormatReaderJson::ProccessValue(const TString& v, bool& worked)
{
    TString val(v.GetTrim());

    worked = true; // Assume it works until we can confirm it doesn't

    // Handle Strings
    if (val.StartsWith(L'\'') && val.EndsWith(L'\''))
        return TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(val.SubString(1, val.GetSize() - 1));

    if (val.StartsWith(L'\"') && val.EndsWith(L'\"'))
        return TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(val.SubString(1, val.GetSize() - 1));

    if (!val.CompareNoCase(L"true"))
        return TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TPrimitiveVariable>(true);

    if (!val.CompareNoCase(L"false"))
        return TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TPrimitiveVariable>(false);

    if (!val.CompareNoCase(L"null"))
        return TrecPointer<TVariable>();

    UINT numVal = 0;
    if (TString::ConvertStringToUint(val, numVal))
        return TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TPrimitiveVariable>(numVal);

    float fVal = 0.0f;
    if (!val.ConvertToFloat(fVal))
        return TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TPrimitiveVariable>(fVal);

    int iVal = 0;
    if (!val.ConvertToInt(iVal))
        return TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TPrimitiveVariable>(iVal);

    // No other options, did not work
    worked = false;
    return TrecPointer<TVariable>();
}

TrecPointer<TFormatReader> TFormatReaderJson::TFormatReaderBuilderJson::GetReader(TrecPointer<TFileShell> file)
{
    if (file.Get() && file->GetName().EndsWith(L".json", true))
        return TrecPointerKey::GetNewTrecPointerAlt<TFormatReader, TFormatReaderJson>(file);
    return TrecPointer<TFormatReader>();
}
