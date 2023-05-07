#pragma once
#include <TEnvironment.h>
#include "AnagameCodeEx.h"
#include <AnagameEnvironment.h>
#include <TcLanguage.h>


class ANAGAME_CODE_EX TAnagameCodeExEnv : public AGProjectEnvironment
{
protected:
    TrecPointer<TVariable> projectVariables;
    TDataMap<TrecPointer<TcLanguage>> languages;
    TDataMap<TString> languageExtensions;

    TrecPointer<TcLanguage> RetrieveLanguage(const TString& fileName);
    bool LoadLanguage(const TString& fileName, TString& language);

    void SaveFileRecord(const TString& fileName);

public:
    TAnagameCodeExEnv(const TString& name, TrecActivePointer<TFileShell> directory);

    virtual TrecPointer<TObject> RetrieveResource(const TString& name) override;
    virtual void RetrieveResourceListSub(TDataArray<TrecPointer<TVariable>>& resources) override;

    virtual TrecPointer<TObjectNode> GetProjectNodes(const TString& name)override;
    virtual void GetProjectNodeTypes(TDataArray<TString>& nodeTypes)override;

    virtual TString Save() override;
    virtual void Refresh() override;

    /**
     * Returns a list of Libraries that should be loaded by the Anagame Builder then a project is loaded
     */
    virtual TDataArray<TString> GetBuilderDependencies() override;

    /**
     * Returns a list of Libraries that should be loaded by the tap once it is launched
     */
    virtual TDataArray<TString> GetDependencies() override;
};


class ANAGAME_CODE_EX TAnagameCodeExBuilder : public TEnvironmentBuilder
{
public:
    virtual TString GetName() override;
    /**
 * Reports the name of the Environment that can support the requested Resource type (empty string if the resource is not supported)
 */
    virtual TString SupportsResource(const TString& resource) override;
    /**
     * Report the name of the Environments, resource TYpes, and Resuorces that can manage that File Type (strings should use 2 semicolons
     * as a separater i.e. '[Environment Name];[Resource Type];[Resource Name]'
     */
    virtual void SupportsFileType(TDataArray<TString>& envResources) override;

    virtual TrecPointer<TEnvironment> BuildEnvironment(TString& name) override;


    virtual TrecPointer<TEnvironment> BuildProjectEnvironment(TString& name, TString& projectName, TrecActivePointer<TFileShell> directory) override;

    virtual void GetProjectTypes(TDataArray<TString>& types) override;

    virtual void GetProjectTypes(TDataArray<TString>& types, TrecActivePointer<TDirectory> workspace) override;

    virtual void SupportsFileType(TDataArray<TString>& types, const TString& ext) override;
};


void ANAGAME_CODE_EX LoadAnagameResources();
void ANAGAME_CODE_EX SetLibraryLocation(TrecPointer<TDirectory> directory);