#pragma once
#include <TPage.h>
#include <TEnvironment.h>
#include "AnaApp.h"
#include "FindTextHandler.h"

using env_target = enum class env_target {
    user,
    anagame,
    project,
    other
};




class _ANA_APP AnafaceBuilder : public TPageBuilder
{
public:
    virtual TrecPointer<TPage> GetPage(const TString& details) override;
    virtual TrecPointer<TPage> GetPage(TrecPointer<TFileShell> file) override;
    virtual TrecPointer<TPage> GetPage() override;
};


class _ANA_APP UserProfileEnvironment : public TEnvironment
{
protected:
    TrecPointer<TObject> RetrieveResource(const TString& name) override;
    void RetrieveResourceListSub(TDataArray<TrecPointer<TVariable>>& resources) override;

public:
    UserProfileEnvironment();
    ~UserProfileEnvironment();

    void SaveProperties() override;
};

class _ANA_APP AppDataEnvironment : public TEnvironment
{
protected:
    TrecPointer<TObject> RetrieveResource(const TString& name) override;
    void RetrieveResourceListSub(TDataArray<TrecPointer<TVariable>>& resources) override;
public:
    AppDataEnvironment();
    ~AppDataEnvironment();
};

class _ANA_APP AGProjectEnvironment : public TEnvironment
{
protected:
    TString projectName;
    TrecPointer<TFileShell> directory;
    //TDataMap<TrecPointer<TObjectNode>> nodes;
public:
    virtual TString Save() = 0;

    AGProjectEnvironment(const TString& name, TrecActivePointer<TFileShell> directory);

    virtual void Refresh() = 0;

    /**
     * Returns a list of Libraries that should be loaded by the Anagame Builder then a project is loaded
     */
    virtual TDataArray<TString> GetBuilderDependencies() = 0;

    /**
     * Returns a list of Libraries that should be loaded by the tap once it is launched
     */
    virtual TDataArray<TString> GetDependencies() = 0;

    TrecPointer<TFileShell> GetDirectory();

    virtual TrecPointer<TObjectNode> GetProjectNodes(const TString& name);
    virtual void GetProjectNodeTypes(TDataArray<TString>& nodeTypes);
};

class _ANA_APP BasicAnagameEnvironment :
    public TEnvironment
{
    TrecPointer<TEnvironment> anagame, user;
    TrecPointer<AGProjectEnvironment> project;

    TrecPointerSoft<FindTextHandler> findTextHandler;
protected:
    TrecPointer<TObject> RetrieveResource(const TString& name) override;
    void RetrieveResourceListSub(TDataArray<TrecPointer<TVariable>>& resources) override;

public:
    TrecPointer<TObject> GetResource(const TString& name) override;
    BasicAnagameEnvironment();
    void SetProject(TrecActivePointer<AGProjectEnvironment> projectEnvironment);
    void SetProperty(const TString& name, TrecPointer<TVariable> var, env_target target);

    TrecPointer<TVariable> GetProperty(const TString& name, env_target target);
    TrecPointer<TVariable> GetProperty(const TString& name, const TDataArray<env_target>& targets);

    TrecPointer<AGProjectEnvironment> RetrieveProjectEnvironment();
};