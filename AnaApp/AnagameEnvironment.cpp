#include <AnafacePage.h>
#include <TFormatReader.h>
#include "AnagameEnvironment.h"

TString resourceAnaface(L"anagame.anaface");

TrecPointer<TObject> BasicAnagameEnvironment::RetrieveResource(const TString& name)
{
	if (name.CompareNoCase(resourceAnaface))
	{
		return TrecPointerKey::GetNewTrecPointerAlt<TObject, AnafaceBuilder>();
	}
}
void BasicAnagameEnvironment::RetrieveResourceListSub(TDataArray<TString>& resources) {
	resources.push_back(resourceAnaface);
}

TrecPointer<TPage> AnafaceBuilder::GetPage(const TString& details)
{
	return TrecPointer<TPage>();
}

TrecPointer<TPage> AnafaceBuilder::GetPage(TrecPointer<TFileShell> file)
{
	TrecPointer<TPage> ret;

	ret = TrecPointerKey::GetNewSelfTrecPointerAlt<TPage, AnafacePage>(this->board, this->handler);

	auto anaRet = TrecPointerKey::ConvertPointer<TPage, AnafacePage>(ret);

	TString result(anaRet->PrepPage(file, handler));

	if (result.GetSize())
	{
		ret.Nullify();
		return ret;
	}

	anaRet->Create(this->space);

	return ret;
}
