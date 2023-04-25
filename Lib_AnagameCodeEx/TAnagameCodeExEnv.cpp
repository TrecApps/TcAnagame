#include "pch.h"
#include "TAnagameCodeExEnv.h"
#include <TInstance.h>
#include "TCodeHandler.h"
#include <TFormatReader.h>

TString anagameCodeName(L"AnaCode");

TAnagameCodeExEnv::TAnagameCodeExEnv(const TString& name, TrecActivePointer<TFileShell> directory) : AGProjectEnvironment(name, directory)
{
}

TrecPointer<TObject> TAnagameCodeExEnv::RetrieveResource(const TString& name)
{
    auto pieces = name.split(L":");

    if(pieces->Size() < 2 || pieces->at(0).Compare(L"AnagameCodeEx"))
        return TrecPointer<TObject>();

    if (!pieces->at(1).CompareNoCase(L"Code_Handler"))
    {
        TrecPointer<TFileShell> dataFile;
        if (pieces->Size() >= 3)
        {
            TString path(this->directory->GetPath() + TC_FILE_SEP + pieces->at(2));
            dataFile = TFileShell::GetFileInfo(path);
            if (!dataFile.Get())
            {
                TFile attemptFile(this->directory, pieces->at(2), TFile::t_file_create_new | TFile::t_file_write);
                attemptFile.Close();
                dataFile = TFileShell::GetFileInfo(path);
            }
        }
        TrecPointer<AnafaceBuilder> builder = TrecPointerKey::GetNewTrecPointer<AnafaceBuilder>();
        builder->SetHandler(TrecPointerKey::GetNewSelfTrecPointerAlt<TPage::EventHandler, TCodeHandler>());


        TString tmlFile(GetDirectoryWithSlash(CentralDirectories::cd_Executable));
        tmlFile.Append(L"UI\\LineTextEditor.json");
        TrecPointer<TFileShell> jsonFile = TFileShell::GetFileInfo(tmlFile);

        builder->SetDataFile(dataFile);
        builder->SetUIFile(jsonFile);
        return TrecPointerKey::ConvertPointer<AnafaceBuilder, TObject>(builder);
    }
    return TrecPointer<TObject>();
}

void TAnagameCodeExEnv::RetrieveResourceListSub(TDataArray<TrecPointer<TVariable>>& resources)
{
    // First Resource, Java Script Files
    TrecPointer<TVariable> baseSpecs = TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TArrayVariable>();
    TrecPointer<TArrayVariable> resourceSpecs = TrecPointerKey::ConvertPointer<TVariable, TArrayVariable>(baseSpecs);

    resourceSpecs->Push(TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(L"JavaScript File"));
    resourceSpecs->Push(TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(L"File-Ext: js"));
    resourceSpecs->Push(TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(L"Env-Source: AnagameCodeEx"));
    resourceSpecs->Push(TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(L"Resource-Type: Page"));
    resourceSpecs->Push(TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(L"Resource: Code_Handler"));
    resourceSpecs->Push(TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(L"IDE_Loc: Code"));

    resources.push_back(baseSpecs);
}

TrecPointer<TObjectNode> TAnagameCodeExEnv::GetProjectNodes(const TString& name)
{
    if (!name.Compare(L"Files"))
    {
        TrecPointer<TObjectNode> ret = TrecPointerKey::GetNewSelfTrecPointerAlt<TObjectNode, TFileNode>(0);
        dynamic_cast<TFileNode*>(ret.Get())->SetFile(this->directory);
        return ret;
    }
    return TrecPointer<TObjectNode>();
}

void TAnagameCodeExEnv::GetProjectNodeTypes(TDataArray<TString>& nodeTypes)
{
    nodeTypes.push_back(L"Files");
}

TString TAnagameCodeExEnv::Save()
{
    TrecPointer<TFileShell> fileShell = TFileShell::GetFileInfo(directory->GetPath() + TC_FILE_SEP + L".Anacode.json");
    if (!fileShell.Get())
    {
        TFile attemptFile(directory, L".Anacode.json", TFile::t_file_create_new | TFile::t_file_write);
        attemptFile.Close();
        fileShell = TFileShell::GetFileInfo(directory->GetPath() + TC_FILE_SEP + L".Anacode.json");
    }
    TrecPointer<TFormatReader> formatWriter = TFormatReader::GetReader(fileShell);
    if (formatWriter.Get())
    {
        formatWriter->Write(this->projectVariables, TFormatReader::write_mode::formatted);
        return L"";
    }
    return L"Could not Save Environment!";
}

void TAnagameCodeExEnv::Refresh()
{
    TrecPointer<TFileShell> fileShell = TFileShell::GetFileInfo(directory->GetPath() + TC_FILE_SEP + L".Anacode.json");
    TrecPointer<TFormatReader> formatWriter = TFormatReader::GetReader(fileShell);
    this->projectVariables.Nullify();
    if (formatWriter.Get())
    {
        TString res(formatWriter->Read());
        if (res.GetSize())
        {


        }
        else
            this->projectVariables = formatWriter->GetData();
    }

    if (!projectVariables.Get())
    {
        this->projectVariables = TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TJsonVariable>();
        auto jsonProjectVars = TrecPointerKey::ConvertPointer<TVariable, TJsonVariable>(projectVariables);

        jsonProjectVars->SetField(L"Name", TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(projectName));
        jsonProjectVars->SetField(L"Files", TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TJsonVariable>());

        Save();
    }
}

TDataArray<TString> TAnagameCodeExEnv::GetBuilderDependencies()
{
    return TDataArray<TString>();
}

TDataArray<TString> TAnagameCodeExEnv::GetDependencies()
{
    return TDataArray<TString>();
}

TString TAnagameCodeExBuilder::GetName()
{
    return L"AnagameCodeEx";
}

TString TAnagameCodeExBuilder::SupportsResource(const TString& resource)
{
    return TString();
}

void TAnagameCodeExBuilder::SupportsFileType(TDataArray<TString>& envResources)
{
    envResources.push_back(L"js:Anagame_Code:Code_Handler");
}

TrecPointer<TEnvironment> TAnagameCodeExBuilder::BuildEnvironment(TString& name)
{
    return TrecPointer<TEnvironment>();
}

TrecPointer<TEnvironment> TAnagameCodeExBuilder::BuildProjectEnvironment(TString& name, TString& projectName, TrecActivePointer<TFileShell> directory)
{
    TrecPointer<TFileShell> direct = directory.GetTrecPointer();
    TrecPointer<TDirectory> actDirectory = TrecPointerKey::ConvertPointer<TFileShell, TDirectory>(direct);
    if (!actDirectory.Get())
        return TrecPointer<TEnvironment>();
    if (!name.Compare(anagameCodeName))
    {
        TString newPath(direct->GetPath() + TC_FILE_SEP + projectName);
        ForgeDirectory(newPath);;
        TrecPointer<TFileShell> newDirectory = TFileShell::GetFileInfo(newPath);

        
        return TrecPointerKey::GetNewTrecPointerAlt<TEnvironment, TAnagameCodeExEnv>(projectName, TrecActivePointer<TFileShell>(newDirectory));
        
    }
}

void TAnagameCodeExBuilder::GetProjectTypes(TDataArray<TString>& types)
{
    types.push_back(anagameCodeName);
}

void TAnagameCodeExBuilder::GetProjectTypes(TDataArray<TString>& types, TrecActivePointer<TDirectory> workspace)
{
    auto work = workspace.GetTrecPointer();
    TDataArray<TrecPointer<TFileShell>> files;
    work->GetFileListing(files);
    for (UINT Rust = 0; Rust < files.Size(); Rust++)
    {
        TString fName(files[Rust]->GetName());
        if (!fName.CompareNoCase(L".Anacode.json"))
        {
            types.push_back(anagameCodeName);
        }
    }
}

void TAnagameCodeExBuilder::SupportsFileType(TDataArray<TString>& types, const TString& ext)
{
    if (!ext.CompareNoCase("js"))
        types.push_back(L"Anagame_Code:Code_Handler");
}

void LoadAnagameResources()
{
    TInstance::GetInstance()->SubmitEnvironmentBuilder(TrecPointerKey::GetNewTrecPointerAlt<TEnvironmentBuilder, TAnagameCodeExBuilder>());
}

void SetLibraryLocation(TrecPointer<TDirectory> directory)
{

}