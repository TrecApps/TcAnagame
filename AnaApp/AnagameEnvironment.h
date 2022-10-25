#include <TEnvironment.h>
#include "AnaApp.h"
class _ANA_APP BasicAnagameEnvironment :
    public TEnvironment
{
protected:
    TrecPointer<TObject> RetrieveResource(const TString& name) override;
    void RetrieveResourceListSub(TDataArray<TString>& resources) override;
};