#include <AnafacePage.h>
#include <TFormatReader.h>
#include "AnagameEnvironment.h"
#include <TFile.h>
#include <TContainerVariable.h>
#include "FileHandler.h"


#ifdef _WINDOWS
WCHAR w = L'\\';
#else
WCHAR w = L'/';
#endif

TrecPointer<TObject> BasicAnagameEnvironment::RetrieveResource(const TString& name)
{
	auto pieces = name.split(L":");

	if (pieces->Size() < 2 || pieces->at(0).Compare(L"anagame"))
		return TrecPointer<TObject>();



	if (!pieces->at(1).CompareNoCase(L"anaface"))
	{
		return TrecPointerKey::GetNewTrecPointerAlt<TObject, AnafaceBuilder>();
	}
	if (!pieces->at(1).CompareNoCase(L"hierarchy") && project.Get() && pieces->Size() >= 3)
	{

		auto hierarchyType = project->GetProjectNodes(pieces->at(2));
		if (hierarchyType.Get())
			return TrecPointer<TObject>();
		auto ret = TrecPointerKey::GetNewTrecPointerAlt<TObject, AnafaceBuilder>();
		auto pRet = TrecPointerKey::ConvertPointer<TObject, AnafaceBuilder>(ret);

		auto fileHandler = TrecPointerKey::GetNewSelfTrecPointerAlt<TPage::EventHandler, FileHandler>();
		dynamic_cast<FileHandler*>(fileHandler.Get())->SetNodes(hierarchyType);
		pRet->SetHandler(fileHandler);
		TrecPointer<TFileShell> fileShell = TFileShell::GetFileInfo(GetDirectoryWithSlash(CentralDirectories::cd_Executable) + L"UI/Hierarchy.json");
		assert(fileShell.Get());
		pRet->SetUIFile(fileShell);

		return ret;
	}
	return TrecPointer<TObject>();
}
void BasicAnagameEnvironment::RetrieveResourceListSub(TDataArray<TrecPointer<TVariable>>& resources) {
	// First Resource, Java Script Files
	TrecPointer<TVariable> baseSpecs = TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TArrayVariable>();
	TrecPointer<TArrayVariable> resourceSpecs = TrecPointerKey::ConvertPointer<TVariable, TArrayVariable>(baseSpecs);

	resourceSpecs->Push(TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(L"Anaface Buider"));
	resourceSpecs->Push(TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(L"No Show"));
	resourceSpecs->Push(TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(L"Env-Source: anagame"));
	resourceSpecs->Push(TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(L"Resource-Type: Page"));
	resourceSpecs->Push(TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(L"Resource: anaface"));

	resources.push_back(baseSpecs);

	

	if (project.Get())
	{
		TDataArray<TString> nodeTypes;
		project->GetProjectNodeTypes(nodeTypes);

		for (UINT Rust = 0; Rust < nodeTypes.Size(); Rust++)
		{	
			baseSpecs = TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TArrayVariable>();
			resourceSpecs = TrecPointerKey::ConvertPointer<TVariable, TArrayVariable>(baseSpecs);

			resourceSpecs->Push(TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(L"Project Hierarchy"));
			resourceSpecs->Push(TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(TString(L"Details: ") + nodeTypes[Rust]));
			resourceSpecs->Push(TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(L"Env-Source: anagame"));
			resourceSpecs->Push(TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(L"Resource-Type: Page"));
			resourceSpecs->Push(TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(L"Resource: hierarchy"));

			resources.push_back(baseSpecs);
		}
	}
}

BasicAnagameEnvironment::BasicAnagameEnvironment()
{
	this->anagame = TrecPointerKey::GetNewTrecPointerAlt<TEnvironment, AppDataEnvironment>();
	this->user = TrecPointerKey::GetNewTrecPointerAlt<TEnvironment, UserProfileEnvironment>();
}

void BasicAnagameEnvironment::SetProject(TrecActivePointer<AGProjectEnvironment> projectEnvironment)
{
	if (this->project.Get())
		this->project->Save();

	this->project = projectEnvironment.GetTrecPointer();
}

void BasicAnagameEnvironment::SetProperty(const TString& name, TrecPointer<TVariable> var, env_target target)
{
	switch (target)
	{
	case env_target::anagame:
		anagame->SetProperty(name, var, true);
		break;
	case env_target::user:
		user->SetProperty(name, var, true);
		break;
	case env_target::project:
		if (project.Get())
			project->SetProperty(name, var, true);
		break;
	default:
		TEnvironment::SetProperty(name, var, true);
	}
}

TrecPointer<AGProjectEnvironment> BasicAnagameEnvironment::RetrieveProjectEnvironment()
{
	return project;
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

	auto pHandler = anaRet->GetHandler();
	if (dataFile.Get())
		pHandler->SetDataFile(dataFile);
	pHandler->Initialize(ret);

	return ret;
}

TrecPointer<TPage> AnafaceBuilder::GetPage()
{
	return GetPage(targetFile);
}

TrecPointer<TObject> UserProfileEnvironment::RetrieveResource(const TString& name)
{
	return TrecPointer<TObject>();
}

void UserProfileEnvironment::RetrieveResourceListSub(TDataArray<TrecPointer<TVariable>>& resources)
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
		assert(reader.Get());
		TString message(reader->Read());
		if (message.GetSize())
			return;
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
	TrecPointer<TVariable> vProps = TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TJsonVariable>();
	TrecPointer<TJsonVariable> props = TrecPointerKey::ConvertPointer<TVariable, TJsonVariable>(vProps);

	TDataEntry<TrecPointer<TVariable>> entry;
	for (UINT Rust = 0; properties.GetEntryAt(Rust, entry); Rust++)
		props->SetField(entry.key, entry.object);


	TString file(GetDirectoryWithSlash(CentralDirectories::cd_User) + L".Anagame" + w + L"settings.json");

	TrecPointer<TFileShell> targetFile = TFileShell::GetFileInfo(file);
	assert(targetFile.Get());
	TrecPointer<TFormatReader> reader = TFormatReader::GetReader(targetFile);
	assert(reader.Get());

	reader->Write(vProps);
	
}

TrecPointer<TObject> AppDataEnvironment::RetrieveResource(const TString& name)
{
	return TrecPointer<TObject>();
}

void AppDataEnvironment::RetrieveResourceListSub(TDataArray<TrecPointer<TVariable>>& resources)
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
		assert(reader.Get());
		TString message(reader->Read());
		if (message.GetSize())
			return;
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
	TrecPointer<TVariable> vProps = TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TJsonVariable>();
	TrecPointer<TJsonVariable> props = TrecPointerKey::ConvertPointer<TVariable, TJsonVariable>(vProps);

	TDataEntry<TrecPointer<TVariable>> entry;
	for (UINT Rust = 0; properties.GetEntryAt(Rust, entry); Rust++)
		props->SetField(entry.key, entry.object);


	TString file(GetDirectoryWithSlash(CentralDirectories::cd_User) + L"Anagame" + w + L"settings.json");

	TrecPointer<TFileShell> targetFile = TFileShell::GetFileInfo(file);
	assert(targetFile.Get());
	TrecPointer<TFormatReader> reader = TFormatReader::GetReader(targetFile);
	assert(reader.Get());

	reader->Write(vProps);
}

AGProjectEnvironment::AGProjectEnvironment(const TString& name, TrecActivePointer<TFileShell> directory)
{
	this->projectName.Set(name);
	this->directory = directory.GetTrecPointer();
}

TrecPointer<TFileShell> AGProjectEnvironment::GetDirectory()
{
	return directory;
}

TrecPointer<TObjectNode> AGProjectEnvironment::GetProjectNodes(const TString& name)
{
	return TrecPointer<TObjectNode>();
}

void AGProjectEnvironment::GetProjectNodeTypes(TDataArray<TString>& nodeTypes)
{
}
