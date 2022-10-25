#include <TPage.h>
#include <TEnvironment.h>
#include "AnaApp.h"
class _ANA_APP BasicAnagameEnvironment :
    public TEnvironment
{
protected:
    TrecPointer<TObject> RetrieveResource(const TString& name) override;
    void RetrieveResourceListSub(TDataArray<TString>& resources) override;
};

class _ANA_APP AnafaceBuilder : public TPageBuilder
{
public:
    virtual TrecPointer<TPage> GetPage(const TString& details) override;
    virtual TrecPointer<TPage> GetPage(TrecPointer<TFileShell> file) override;
};