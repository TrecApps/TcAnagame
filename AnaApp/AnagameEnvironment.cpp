#include <AnafacePage.h>
#include <TFormatReader.h>
#include "AnagameEnvironment.h"
#include <TFile.h>
#include <TContainerVariable.h>

TString resourceAnaface(L"anagame.anaface");

#ifdef _WINDOWS
WCHAR w = L'\\';
#else
WCHAR w = L'/';
#endif

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

TrecPointer<TObject> UserProfileEnvironment::RetrieveResource(const TString& name)
{
	return TrecPointer<TObject>();
}

void UserProfileEnvironment::RetrieveResourceListSub(TDataArray<TString>& resources)
{
}

UserProfileEnvironment::UserProfileEnvironment()
{
	TString file(GetDirectoryWithSlash(CentralDirectories::cd_User) + L".Anagame");

	ForgeDirectory(file);

	TrecPointer<TFileShell> targetFile = TFileShell::GetFileInfo(file + w + L"settings.json");

	if (!targetFile.Get())
	{
		TrecPointer<TFileShell> userSettingsDirectory = TFileShell::GetFileInfo(file);
		assert(userSettingsDirectory.Get() && userSettingsDirectory->IsDirectory());

		TFile creat(userSettingsDirectory, L"settings.json", TFile::t_file_create_always | TFile::t_file_write);

	}
	else
	{
		TrecPointer<TFormatReader> reader = TFormatReader::GetReader(targetFile);
		assert(reader.Get() && !reader->Read().GetSize());
		TrecPointer<TVariable> data = reader->GetData();

		assert(data.Get() && data->GetVarType() == var_type::json);
		TrecPointer<TJsonVariable> jData = TrecPointerKey::ConvertPointer<TVariable, TJsonVariable>(data);
		TString fieldName;
		
		for (UINT Rust = 0; jData->RetrieveFieldAt(Rust, fieldName, data); Rust++)
		{
			this->SetProperty(fieldName, data);
		}
	}
}

UserProfileEnvironment::~UserProfileEnvironment()
{
	TrecPointer<TJsonVariable> props = TrecPointerKey::GetNewSelfTrecPointer<TJsonVariable>();

	TDataEntry<TrecPointer<TVariable>> entry;
	for (UINT Rust = 0; properties.GetEntryAt(Rust, entry); Rust++)
		props->SetField(entry.key, entry.object);


	TString file(GetDirectoryWithSlash(CentralDirectories::cd_User) + L".Anagame" + w + L"settings.json");

	TrecPointer<TFileShell> targetFile = TFileShell::GetFileInfo(file);
	assert(targetFile.Get());
	TrecPointer<TFormatReader> reader = TFormatReader::GetReader(targetFile);
	assert(reader.Get());

	reader->Write(TrecPointerKey::ConvertPointer<TJsonVariable, TVariable>(props));
	
}

TrecPointer<TObject> AppDataEnvironment::RetrieveResource(const TString& name)
{
	return TrecPointer<TObject>();
}

void AppDataEnvironment::RetrieveResourceListSub(TDataArray<TString>& resources)
{
}

AppDataEnvironment::AppDataEnvironment()
{
	TString file(GetDirectoryWithSlash(CentralDirectories::cd_AppData) + L"Anagame");

	ForgeDirectory(file);

	TrecPointer<TFileShell> targetFile = TFileShell::GetFileInfo(file + w + L"settings.json");

	if (!targetFile.Get())
	{
		TrecPointer<TFileShell> userSettingsDirectory = TFileShell::GetFileInfo(file);
		assert(userSettingsDirectory.Get() && userSettingsDirectory->IsDirectory());

		TFile creat(userSettingsDirectory, L"settings.json", TFile::t_file_create_always | TFile::t_file_write);

	}
	else
	{
		TrecPointer<TFormatReader> reader = TFormatReader::GetReader(targetFile);
		assert(reader.Get() && !reader->Read().GetSize());
		TrecPointer<TVariable> data = reader->GetData();

		assert(data.Get() && data->GetVarType() == var_type::json);
		TrecPointer<TJsonVariable> jData = TrecPointerKey::ConvertPointer<TVariable, TJsonVariable>(data);
		TString fieldName;

		for (UINT Rust = 0; jData->RetrieveFieldAt(Rust, fieldName, data); Rust++)
		{
			this->SetProperty(fieldName, data);
		}
	}
}

AppDataEnvironment::~AppDataEnvironment()
{
	TrecPointer<TJsonVariable> props = TrecPointerKey::GetNewSelfTrecPointer<TJsonVariable>();

	TDataEntry<TrecPointer<TVariable>> entry;
	for (UINT Rust = 0; properties.GetEntryAt(Rust, entry); Rust++)
		props->SetField(entry.key, entry.object);


	TString file(GetDirectoryWithSlash(CentralDirectories::cd_User) + L"Anagame" + w + L"settings.json");

	TrecPointer<TFileShell> targetFile = TFileShell::GetFileInfo(file);
	assert(targetFile.Get());
	TrecPointer<TFormatReader> reader = TFormatReader::GetReader(targetFile);
	assert(reader.Get());

	reader->Write(TrecPointerKey::ConvertPointer<TJsonVariable, TVariable>(props));
}
