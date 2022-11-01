#pragma once
#include "TString.h"
#include "TDataArray.h"
#include "TDataMap.h"

class _TREC_LIB_DLL TEnvironment :
    public TObject
{
    TDataArray<TrecPointer<TEnvironment>> childEnvironments;
protected:
    TDataMap<TrecPointer<TVariable>> properties;
    virtual TrecPointer<TObject> RetrieveResource(const TString& name) = 0;
    virtual void RetrieveResourceListSub(TDataArray<TString>& resources) = 0;
public:
    TEnvironment() = default;

    ObjectType GetObjectType()final;

    void RetrieveResourceList(TDataArray<TString>& resources);

    void AddEnvironment(TrecPointer<TEnvironment> childEnvironment);

    TrecPointer<TObject> GetResource(const TString& name);

    TrecPointer<TVariable> GetProperty(const TString& name)const ;

    void SetProperty(const TString& name, TrecPointer<TVariable> var, bool doOverride = false);

    TrecPointer<TEnvironment> GetChildEnvironmentAt(UINT index) const;

    UINT ChildEnvironmentCount()const;
};

