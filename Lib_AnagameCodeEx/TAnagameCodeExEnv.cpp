#include "pch.h"
#include "TAnagameCodeExEnv.h"
#include <TInstance.h>
#include "TCodeHandler.h"
#include <TFormatReader.h>
#include <TcCompiler.h>

TString anagameCodeName(L"AnaCode");

TrecPointer<TcLanguage> TAnagameCodeExEnv::RetrieveLanguage(const TString& fileName)
{
    TString extension(fileName.SubString(fileName.FindLast(L".") + 1));
    TString language;
    TrecPointer<TcLanguage> ret;
    if (languageExtensions.retrieveEntry(extension, language) || LoadLanguage(extension, language))
    {
        languages.retrieveEntry(language, ret);
    }

    return ret;
}

bool TAnagameCodeExEnv::LoadLanguage(const TString& fileName, TString& language)
{
    TrecPointer<TDirectory> languagesFolder = TrecPointerKey::ConvertPointer<TFileShell, TDirectory>( TFileShell::GetFileInfo(
        GetDirectoryWithSlash(CentralDirectories::cd_Executable) + L"Languages"
    ));
    assert(languagesFolder.Get());

    TDataArray<TrecPointer<TFileShell>> languageFiles;
    languagesFolder->GetFileListing(languageFiles);

    for (UINT Rust = 0; Rust < languageFiles.Size(); Rust++) {
        auto languageFile = languageFiles[Rust];

        TString name(languageFile->GetName());
        auto namePieces = name.split(L".");
        assert(namePieces.Get() && namePieces->Size() >= 2);
        TString languageName(namePieces->at(0));
        auto extensions = namePieces->at(1).splitn(L"-");
        
        bool found = false;
        for (UINT C = 0; !found && C < extensions->Size(); C++) {
            if (extensions->at(C).CompareNoCase(fileName))
            {
                found = true;
            }
        }

        if (!found) continue;

        // First Attempt to generate the language object
        TrecPointer<TFormatReader> reader = TFormatReader::GetReader(languageFile);
        assert(reader.Get());
        reader->Read();

        TrecPointer<TcLanguage> newLanguage = TrecPointerKey::GetNewTrecPointer<TcLanguage>( 
            TrecPointerKey::ConvertPointer<TVariable, TJsonVariable>(reader->GetData())
        );

        TString langResult(newLanguage->Init());
        if (langResult.GetSize())
            return false;

        // It works
        for (UINT C = 0; !found && C < extensions->Size(); C++)
        {
            this->languageExtensions.addEntry(extensions->at(C), name);
        }
        language.Set(name);
        this->languages.addEntry(language, newLanguage);
        return true;
    }

    return false;
}

void TAnagameCodeExEnv::SaveFileRecord(const TString& fileName)
{
    if (!projectVariables.Get())
        projectVariables = TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TJsonVariable>();

    auto jProjVariables = TrecPointerKey::ConvertPointer<TVariable, TJsonVariable>(projectVariables);

    assert(jProjVariables.Get());

    TrecPointer<TVariable> fileList;

    if (!jProjVariables->RetrieveField(L"Files", fileList) || fileList->GetVarType() != var_type::list)
    {
        fileList = TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TArrayVariable>();
        jProjVariables->SetField(L"Files",fileList);
    }

    auto jFileList = TrecPointerKey::ConvertPointer<TVariable, TArrayVariable>(fileList);

    TrecPointer<TVariable> fileNameElement;
    for (UINT Rust = 0; jFileList->GetValueAt(Rust, fileNameElement); Rust++)
    {
        TString actName(TStringVariable::Extract(fileNameElement, L""));
        if (!actName.Compare(fileName))return;
    }

    jFileList->Push(TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(fileName));
}

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

            SaveFileRecord(pieces->at(2));
            Save();
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

    if (!pieces->at(1).CompareNoCase(L"AG_Compiler") && pieces->Size() >= 3)
    {
        TrecPointer<TcLanguage> language = RetrieveLanguage(pieces->at(2));
        if (language.Get())
        {
            TrecPointer<TFileShell> file = TFileShell::GetFileInfo(pieces->at(2));
            if (!file.Get())
            {
                TString path(this->directory->GetPath() + TC_FILE_SEP + pieces->at(2));
                file = TFileShell::GetFileInfo(path);
            }

            if (file.Get())
            {
                TrecPointer<TEnvironment> thisEnv = TrecPointerKey::TrecFromSoft<>(this->self);

                TrecPointer<TcCompiler> compiler = TrecPointerKey::GetNewTrecPointer<TcCompiler>(
                    TrecActivePointer<TFileShell>(file),
                    TrecActivePointer<TEnvironment>(thisEnv),
                    TrecActivePointer<TcLanguage>(language)
                );
                return TrecPointerKey::ConvertPointer<TcCompiler, TObject>(compiler);
            }
        }
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
        jsonProjectVars->SetField(L"Files", TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TArrayVariable>());

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

        
        return TrecPointerKey::GetNewSelfTrecPointerAlt<TEnvironment, TAnagameCodeExEnv>(projectName, TrecActivePointer<TFileShell>(newDirectory));
        
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