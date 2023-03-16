#include "TDialog.h"
#include <nfd.h>


TDialog::TDialog(TrecPointer<TWindow> parent, GLFWwindow* win): TWindow(win)
{
	this->parent = parent;
	mode = t_dialog_modal_mode::no_modal;
	dialogRunning = true;
}

void TDialog::SetModalMode(t_dialog_modal_mode newMode)
{
	if (parent.Get())
		mode = newMode;
}

void TDialog::SetCloseCallBack(TrecPointer<TcProcedureRunner> runner)
{
	this->closeCallback = runner;
}

TrecPointer<TVariable> TDialog::GetResult()
{
	return result;
}

void TDialog::SetResult(TrecPointer<TVariable> result)
{
	this->result = result;
}

bool TDialog::Close()
{
	if (closeCallback.Get())
	{
		TDataArray<TrecPointer<TVariable>> vars;
		vars.push_back(result);
		closeCallback->SetIntialVariables(vars);
		ReturnObject ret;
		closeCallback->Run(ret);
	}

	this->mainPage.Nullify();

	dialogRunning = false;
	/*glfwSetWindowShouldClose(window, 1);
	window = nullptr;*/
	return TWindow::Close();
}

bool TDialog::RunModal()
{
	if (mode == t_dialog_modal_mode::no_modal)
		return false;
	parent->LockWindow();
	if (mode == t_dialog_modal_mode::hard_modal)
	{
		glfwSetWindowAttrib(parent->GetGlfwWindow(), GLFW_RESIZABLE, GLFW_FALSE);
	}
	while (this->dialogRunning)
	{
		TInstance::GetInstance()->DoDraw();
		glfwPollEvents();
	}

	if (mode == t_dialog_modal_mode::hard_modal)
	{
		glfwSetWindowAttrib(parent->GetGlfwWindow(), GLFW_RESIZABLE, GLFW_TRUE);
	}
	parent->UnlockWindow();
	return true;
}


TrecPointer<TWindow> TDialog::GetParentWindow()
{
	return parent;
}

bool IsValidFileExtParam(const TString& param)
{
	bool noComma = true;
	for (UINT Rust = 0; Rust < param.GetSize(); Rust++)
	{
		WCHAR ch = param[Rust];
		ch = towlower(ch);
		if (!((ch >= L'a' && ch <= L'z') || (ch >= L'0' && ch <= L'9') || ch == L','))
			return false;

		if (ch == L',' && noComma)
			return false;
		noComma = (ch == L',');
	}
	return true;
}

bool GenerateFileInfo(TString& error, TrecPointer<TDataArray<TString>> pieces, TDataArray<std::string>& fileParams, TDataArray< nfdfilteritem_t>& filterItems)
{
	for (UINT Rust = 0; Rust < pieces->Size(); Rust++)
	{
		if (Rust % 2 && !IsValidFileExtParam(pieces->at(Rust)))
		{
			error.Format(L"Extension List %ws must only have alpha-numeric characters and comma's as dividers!", pieces->at(Rust).GetConstantBuffer().getBuffer());
			return false;
		}

		fileParams.push_back(pieces->at(Rust).GetRegString());
	}

	assert(!(fileParams.Size() % 2));

	for (UINT Rust = 1; Rust < fileParams.Size(); Rust += 2)
	{
		filterItems.push_back({ fileParams[Rust - 1].c_str(), fileParams[Rust].c_str() });
	}
	return true;
}

TrecPointer<TFileShell> OpenLoadFileDialog(const TString& fileInfo, TrecPointer<TDirectory> dir, TString& error)
{
	error.Empty();
	auto pieces = fileInfo.split(L';');

	TrecPointer<TFileShell> ret;

	if (pieces->Size() % 2)
	{
		error.Set(L"File Info needs to be Set with an even number of peices dividable by ';'");
		return ret;
	}
	TDataArray<std::string> fileParams;
	TDataArray< nfdfilteritem_t> filterItems;
	if (!GenerateFileInfo(error, pieces, fileParams, filterItems))
		return ret;

	nfdchar_t* outPath = nullptr;

	std::string openDirectory = dir.Get() ? dir->GetPath().GetRegString() : std::string();

	nfdresult_t result = NFD_Init();
	if (result != NFD_OKAY)
	{
		error.Set(L"Failed to Launch File Dialog Library!");
		return ret;
	}

	result = NFD_OpenDialog(&outPath, filterItems.data(), filterItems.Size(), dir.Get() ? openDirectory.c_str() : nullptr);
	
	if (result == NFD_OKAY)
	{
		TString path(outPath);
		NFD_FreePath(outPath);
		outPath = nullptr;
		ret = TFileShell::GetFileInfo(path);
		if (!ret.Get())
			error.Format(L"Retrieved Path '%ws' but could not generate File MetaData", path.GetConstantBuffer().getBuffer());
	}
	else if (result == NFD_ERROR)
	{
		error.Set(L"Programmatic Error Occured with Load File Dialog!");
	}
	
	NFD_Quit();
	return ret;
}

bool OpenSaveFileDialog(const TString& fileInfo, TrecPointer<TDirectory> dir, const TString& name, TString& error)
{
	error.Empty();
	auto pieces = fileInfo.split(L';');


	if (pieces->Size() % 2)
	{
		error.Set(L"File Info needs to be Set with an even number of peices dividable by ';'");
		return false;
	}
	TDataArray<std::string> fileParams;
	TDataArray< nfdfilteritem_t> filterItems;
	if (!GenerateFileInfo(error, pieces, fileParams, filterItems))
		return false;

	nfdchar_t* outPath = nullptr;

	std::string openDirectory = dir.Get() ? dir->GetPath().GetRegString() : std::string();

	nfdresult_t result = NFD_Init();
	if (result != NFD_OKAY)
	{
		error.Set(L"Failed to Launch File Dialog Library!");
		return false;
	}

	std::string nameU8(name.GetRegString());

	result = NFD_SaveDialogU8(&outPath, filterItems.data(), filterItems.Size(), dir.Get() ? openDirectory.c_str() : nullptr, nameU8.size() ? nameU8.c_str() : nullptr);

	bool ret = true;

	if (result == NFD_OKAY)
	{
		error.Set(outPath);
		NFD_FreePath(outPath);
		outPath = nullptr;
	}
	else if (result == NFD_ERROR)
	{
		error.Set(L"Programmatic Error Occured with Save File Dialog!");
		ret = false;
	}

	NFD_Quit();
	return ret;
}

TrecPointer<TFileShell> OpenDirectoryDialog(TrecPointer<TDirectory> dir, TString& error)
{
	error.Empty();
	TrecPointer<TFileShell> ret;

	nfdnchar_t* outPath = nullptr;

	TString openDirectory( dir.Get() ? dir->GetPath() : L"");

	nfdresult_t result = NFD_Init();
	if (result != NFD_OKAY)
	{
		error.Set(L"Failed to Launch File Dialog Library!");
		return ret;
	}

	result = NFD_PickFolderN(&outPath, openDirectory.GetSize() ? openDirectory.GetConstantBuffer().getBuffer() : nullptr);

	if (result == NFD_OKAY)
	{
		TString path(outPath);
		NFD_FreePathN(outPath);
		outPath = nullptr;
		ret = TFileShell::GetFileInfo(path);
		if (!ret.Get())
			error.Format(L"Retrieved Path '%ws' but could not generate Folder MetaData", path.GetConstantBuffer().getBuffer());
	}
	else if (result == NFD_ERROR)
	{
		error.Set(L"Programmatic Error Occured with Load Folder Dialog!");
	}

	NFD_Quit();
	return ret;

}
