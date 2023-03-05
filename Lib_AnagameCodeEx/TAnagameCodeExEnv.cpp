#include "pch.h"
#include "TAnagameCodeExEnv.h"
#include <TInstance.h>

TString anagameCodeName(L"AnaCode");

TAnagameCodeExEnv::TAnagameCodeExEnv(const TString& name, TrecActivePointer<TFileShell> directory) : AGProjectEnvironment(name, directory)
{
}

TrecPointer<TObject> TAnagameCodeExEnv::RetrieveResource(const TString& name)
{
    return TrecPointer<TObject>();
}

void TAnagameCodeExEnv::RetrieveResourceListSub(TDataArray<TString>& resources)
{
}

TString TAnagameCodeExEnv::Save()
{
    return TString();
}

void TAnagameCodeExEnv::Refresh()
{
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
        if (fName.EndsWith(L".projtml"))
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