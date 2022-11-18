#include <TPage.h>
#include <TEnvironment.h>
#include "AnaApp.h"

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
};


class _ANA_APP UserProfileEnvironment : public TEnvironment
{
protected:
    TrecPointer<TObject> RetrieveResource(const TString& name) override;
    void RetrieveResourceListSub(TDataArray<TString>& resources) override;

public:
    UserProfileEnvironment();
    ~UserProfileEnvironment();
};

class _ANA_APP AppDataEnvironment : public TEnvironment
{
protected:
    TrecPointer<TObject> RetrieveResource(const TString& name) override;
    void RetrieveResourceListSub(TDataArray<TString>& resources) override;
public:
    AppDataEnvironment();
    ~AppDataEnvironment();
};

class _ANA_APP AGProjectEnvironment : public TEnvironment
{
protected:
    TString projectName;
    TrecPointer<TFileShell> directory;
public:
    virtual TString Save() = 0;

    AGProjectEnvironment(const TString& name, TrecActivePointer<TFileShell> directory);
};

class _ANA_APP BasicAnagameEnvironment :
    public TEnvironment
{
    TrecPointer<TEnvironment> anagame, user;
    TrecPointer<AGProjectEnvironment> project;
protected:
    TrecPointer<TObject> RetrieveResource(const TString& name) override;
    void RetrieveResourceListSub(TDataArray<TString>& resources) override;

public:
    BasicAnagameEnvironment();
    void SetProject(TrecActivePointer<AGProjectEnvironment> projectEnvironment);
    void SetProperty(const TString& name, TrecPointer<TVariable> var, env_target target);

};