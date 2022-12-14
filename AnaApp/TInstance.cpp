#include <GLFW/glfw3.h>
#include "TDialog.h"
#include "TIdeWindow.h"
#include <TFormatReader.h>
#include <TContainerVariable.h>


TrecPointer<TInstance> theInstance;

typedef void(__cdecl* LOAD_PROC)();


TrecPointer<TWindow> TInstance::GetWindow(GLFWwindow* win)
{
	for (UINT Rust = 0; Rust < windows.Size(); Rust++)
	{
		if (windows[Rust].Get() && windows[Rust]->IsWindow(win))
			return windows[Rust];
	}
	return TrecPointer<TWindow>();
}

TInstance::TInstance()
{



	glfwInitResult = glfwInit();



	charFunction = nullptr;
	cursorFunction = nullptr;
	keyFunction = nullptr;
	mouseFunction = nullptr;
	scrollFunction = nullptr;
	focusFunction = nullptr;
	resizeFunction = nullptr;

	windowCount = 0;

	ReadLibraryList();
}

void TInstance::ReadLibraryList()
{
	TString exec = GetDirectoryWithSlash(CentralDirectories::cd_Executable);
	exec += "Libraries/";
	ForgeDirectory(exec);
	libraryFolder = TFileShell::GetFileInfo(exec);
	assert(libraryFolder.Get() && libraryFolder->IsDirectory());

	TrecPointer<TFormatReader> indexReader = TFormatReader::GetReader(TFileShell::GetFileInfo(libraryFolder->GetPath() + L'/' + L"index.json"));

	if (indexReader.Get())
	{
		if(indexReader->Read().GetSize())return;
		auto data = indexReader->GetData();

		TrecPointer<TJsonVariable> objectLib = TrecPointerKey::ConvertPointer<TVariable, TJsonVariable>(data);
		if (!objectLib.Get())
			return;
		objectLib->RetrieveField(L"Libraries", data);
		TrecPointer<TArrayVariable> arrayLib = TrecPointerKey::ConvertPointer<TVariable, TArrayVariable>(data);
		if (!arrayLib.Get()) return;
		for (UINT Rust = 0; arrayLib->GetValueAt(Rust, data); Rust++)
		{
			objectLib = TrecPointerKey::ConvertPointer<TVariable, TJsonVariable>(data);
			if (!objectLib.Get())continue;


		}
	}
	else
	{
		TFile indexFile(libraryFolder, L"index.json", TFile::t_file_create_new | TFile::t_file_write);
		if (indexFile.IsOpen())
			indexFile.WriteString(L"{\n\t\n}");
	}
}

TrecPointer<TInstance> TInstance::GetInstance()
{
	if (!theInstance.Get())
		theInstance = TrecPointerKey::GetNewTrecPointer<TInstance>();
	return theInstance;
}

TInstance::~TInstance()
{
	if (glfwInitResult)
		glfwTerminate();
}

void TInstance::SubmitEnvironmentBuilder(TrecPointer<TEnvironmentBuilder> builder)
{
	if (builder.Get())
		this->environmentBuilders.push_back(builder);
}

bool TInstance::ScanForResource(const TString& resource, TrecPointer<TEnvironment> env)
{
	if(env.Get() && env->GetResource(resource).Get())
		return true;
	TString strEnv;
	for (UINT Rust = 0; !strEnv.GetSize() && Rust < environmentBuilders.Size(); Rust++)
	{
		strEnv.Set(environmentBuilders[Rust]->SupportsResource(resource));
		if (strEnv.GetSize() && env.Get())
		{
			TrecPointer<TEnvironment> newEnv = environmentBuilders[Rust]->BuildEnvironment(strEnv);
			if (newEnv.Get())
				env->AddEnvironment(newEnv);
		}
	}

	return strEnv.GetSize();
}

TDataArray<TString> TInstance::LoadLibraries(const TDataArray<TString>& libraries)
{
	TDataArray<TString> ret;

	for (UINT Rust = 0; Rust < libraries.Size(); Rust++)
	{
		bool found = false;
		for (UINT C = 0; !found && C < this->libraryList.Size(); C++)
		{
			if (libraries[Rust].Compare(libraryList[C].GetName()))
			{
				found = true;

				if (!libraryList[C].Load(this->libraryFolder))
					ret.push_back(libraries[Rust]);
			}
		}
		if (!found)
			ret.push_back(libraries[Rust]);
	}

	return ret;
}

void TInstance::SetSelf(TrecPointer<TInstance> newSelf)
{
	if (this != newSelf.Get())
		throw 1;
	self = TrecPointerKey::SoftFromTrec<>(newSelf);
}

void TInstance::DoDraw()
{
	for (UINT Rust = 0; Rust < windows.Size(); Rust++)
	{
		if (windows[Rust].Get())
			windows[Rust]->OnDraw();
	}
}

int TInstance::GetGlfwInitResult()
{
	return glfwInitResult;
}

UINT TInstance::HasWindows()
{
	return windowCount;
}

UINT TInstance::GenerateWindow(TrecPointer<TWindow>& window, TrecPointer<TFileShell> uiInterface, const TString& name, t_window_type type)
{
	TString useName(name);
	if (!useName.GetSize())
		useName.Set(L"Anagame Window");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	GLFWwindow* glfwWindow = glfwCreateWindow(800,600, useName.GetRegString().c_str(), nullptr, nullptr);
	if (!glfwWindow)
		return 1;

	TrecPointer<DrawingBoard> board;
	
	switch (type)
	{
	case t_window_type::t_window_type_ide:
		board = TrecPointerKey::GetNewSelfTrecPointerAlt<DrawingBoard, TIdeWindow>(glfwWindow);
		break;
	default:
		board = TrecPointerKey::GetNewSelfTrecPointerAlt<DrawingBoard, TWindow>(glfwWindow);
	}
	// To-Do: Other Initialization




	// End To-Do: Assuming initialization worked

	window = TrecPointerKey::ConvertPointer<DrawingBoard, TWindow>(board);

	UINT Rust = 0;
	for (; Rust < windows.Size(); Rust++)
	{
		if (!windows[Rust].Get())
		{
			windows[Rust] = window;
			break;
		}
	}

	if (Rust == windows.Size())
		windows.push_back(window);

	//window->instance = self;

	windowCount++;

	if (charFunction)
		glfwSetCharCallback(glfwWindow, charFunction);
	if (cursorFunction)
		glfwSetCursorPosCallback(glfwWindow, cursorFunction);
	if (keyFunction)
		glfwSetKeyCallback(glfwWindow, keyFunction);
	if (mouseFunction)
		glfwSetMouseButtonCallback(glfwWindow, mouseFunction);
	if (scrollFunction)
		glfwSetScrollCallback(glfwWindow, scrollFunction);
	if (focusFunction)
		glfwSetWindowFocusCallback(glfwWindow, focusFunction);
	if (resizeFunction)
		glfwSetWindowSizeCallback(glfwWindow, resizeFunction);

	if (closeFunction)
		glfwSetWindowCloseCallback(glfwWindow, closeFunction);

	return Rust;
}

UINT TInstance::GenerateDialog(TrecPointer<TWindow>& window, TrecPointer<TWindow> parent,
	const TString& name, TrecPointer<TPageBuilder> page,
	TrecPointer<TFileShell> uiInterface, const TString& details, t_dialog_modal_mode modalMode)
{
	TString useName(name);
	if (!useName.GetSize())
		useName.Set(L"Anagame Dialog");
	return 0;
	GLFWwindow* glfwWindow = glfwCreateWindow(640, 480, useName.GetRegString().c_str(), nullptr, nullptr);
	if (!glfwWindow)
		return 1;

	TrecPointer<DrawingBoard> board = TrecPointerKey::GetNewSelfTrecPointerAlt<DrawingBoard, TDialog>(parent, glfwWindow);

	window = TrecPointerKey::ConvertPointer<DrawingBoard, TWindow>(board);

	UINT Rust = 0;
	for (; Rust < windows.Size(); Rust++)
	{
		if (!windows[Rust].Get())
		{
			windows[Rust] = window;
			break;
		}
	}

	if (Rust == windows.Size())
		windows.push_back(window);

	//window->instance = self;
	if (page.Get())
	{
		page->SetDrawingBoard(board);
		page->SetSpace(board->GetArea());
		window->SetMainPage(uiInterface.Get() ? page->GetPage(uiInterface) : page->GetPage(details));
	}


	

	windowCount++;

	if (charFunction)
		glfwSetCharCallback(glfwWindow, charFunction);
	if (cursorFunction)
		glfwSetCursorPosCallback(glfwWindow, cursorFunction);
	if (keyFunction)
		glfwSetKeyCallback(glfwWindow, keyFunction);
	if (mouseFunction)
		glfwSetMouseButtonCallback(glfwWindow, mouseFunction);
	if (scrollFunction)
		glfwSetScrollCallback(glfwWindow, scrollFunction);
	if (focusFunction)
		glfwSetWindowFocusCallback(glfwWindow, focusFunction);
	if (resizeFunction)
		glfwSetWindowSizeCallback(glfwWindow, resizeFunction);

	if (closeFunction)
		glfwSetWindowCloseCallback(glfwWindow, closeFunction);

	return Rust;
}

void TInstance::SetCallbacks(GLFWcharfun charFun, GLFWcursorposfun cursorFun,
	GLFWkeyfun keyFun, GLFWmousebuttonfun mouseFun,
	GLFWscrollfun scrollFun, GLFWwindowfocusfun focusFun,
	GLFWwindowsizefun resizeFun, GLFWwindowclosefun closeFun) {

	charFunction = charFun;
	cursorFunction = cursorFun;
	keyFunction = keyFun;
	mouseFunction = mouseFun;
	scrollFunction = scrollFun;
	focusFunction = focusFun;
	resizeFunction = resizeFun;
	closeFunction = closeFun;
}

void TInstance::OnChar(GLFWwindow* window, UINT ch)
{
	auto win = GetWindow(window);
	if (win.Get())
		win->OnChar(ch);
}

void TInstance::OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{
	auto win = GetWindow(window);
	if (win.Get())
		win->OnMouseMove(xpos, ypos);
}

void TInstance::OnLButtonDown(GLFWwindow* window, int mods)
{
	auto win = GetWindow(window);
	if (win.Get())
		win->OnLButtonDown(mods);
}

void TInstance::OnLButtonUp(GLFWwindow* window, int mods)
{
	auto win = GetWindow(window);
	if (win.Get())
		win->OnLButtonUp(mods);
}

void TInstance::OnRButtonDown(GLFWwindow* window, int mods)
{
	auto win = GetWindow(window);
	if (win.Get())
		win->OnRButtonDown(mods);
}

void TInstance::OnRButtonUp(GLFWwindow* window, int mods)
{
	auto win = GetWindow(window);
	if (win.Get())
		win->OnLButtonUp(mods);
}

void TInstance::OnScroll(GLFWwindow* window, double x, double y)
{
	auto win = GetWindow(window);
	if (win.Get())
		win->OnScroll(x, y);
}

void TInstance::OnFocus(GLFWwindow* window)
{
	auto win = GetWindow(window);
	if (win.Get())
		win->OnFocus();
}

void TInstance::OnLoseFocus(GLFWwindow* window)
{
	auto win = GetWindow(window);
	if (win.Get())
		win->OnLoseFocus();
}

void TInstance::OnWindowResize(GLFWwindow* window, int w, int h)
{
	auto win = GetWindow(window);
	if (win.Get())
		win->OnResize(w, h);
}

void TInstance::OnWindowClose(GLFWwindow* window)
{
	if (!window)
		return;
	for (UINT Rust = 0; Rust < windows.Size(); Rust++)
	{
		if (!windows[Rust].Get()) continue;
		if (windows[Rust]->IsWindow(window) && windows[Rust]->Close())
		{
			windowCount--;
			windows[Rust].Nullify();
			return;
		}
	}
}

bool TLibrary::ScanProjectFiles(TrecPointer<TFileShell> file)
{
	if(!file.Get())
	return false;

	TString name(file->GetName());
	for (UINT Rust = 0; Rust < projectFiles.Size(); Rust++)
		if (!name.Compare(projectFiles[Rust]))
			return true;
	return false;
}

bool TLibrary::ScanProjectExts(TrecPointer<TFileShell> file)
{
	if (!file.Get())
	return false;

	TString name(file->GetName());
	int lastDot = name.FindLast(L'.');
	if (lastDot < 0)
		return false;
	name.Set(name.SubString(lastDot));
	for (UINT Rust = 0; Rust < projectFileExtensions.Size(); Rust++)
		if (!name.Compare(projectFileExtensions[Rust]))
			return true;
	return false;
}

TLibrary::TLibrary()
{
	libraryHandle = nullptr;
}

void TLibrary::SetName(const TString& name)
{
	this->name.Set(name);
}

TString TLibrary::GetName()
{
	return name;
}

void TLibrary::AppendProjectFile(const TString& fileName)
{
	this->projectFiles.push_back(fileName);
}

void TLibrary::AppendProjectExt(const TString& fileExt)
{
	this->projectFileExtensions.push_back(fileExt);
}

bool TLibrary::SupportsProjectDirectory(TrecPointer<TFileShell> directory)
{
	TrecPointer<TDirectory> realDirectory = TrecPointerKey::ConvertPointer<TFileShell, TDirectory>(directory);
	if(!realDirectory.Get())
		return false;

	TDataArray<TrecPointer<TFileShell>> files = realDirectory->GetFileListing();
	for (UINT Rust = 0; Rust < files.Size(); Rust++)
	{
		if (ScanProjectFiles(files[Rust]) && ScanProjectExts(files[Rust]))
			return true;
	}
	return false;
}

bool TLibrary::IsLoaded()
{
	return this->libraryHandle != nullptr;
}

bool TLibrary::Load(TrecPointer<TFileShell> directory)
{
	if (IsLoaded())
		return true;
	if (!directory.Get())
		return false;
	TrecPointer<TFileShell> targetLibrary = TFileShell::GetFileInfo(directory->GetPath() + L'/' + name);
	if (!targetLibrary.Get())
		return false;
	
#ifdef _WINDOWS
	libraryHandle = LoadLibraryW(targetLibrary->GetPath().GetConstantBuffer().getBuffer());
	if (!libraryHandle)
		return false;
	LOAD_PROC proc = (LOAD_PROC) GetProcAddress(libraryHandle, "LoadAnagameResources");
	if (!proc)
		return false;
	proc();
#elif defined(__linux__) || (defined (__APPLE__) && defined (__MACH__))
	libraryHandle = dlopen(targetLibrary->GetPath().GetRegString().c_str(), RTLD_NOW);
	if (!libraryHandle)
		return false;
	dlerror();
	LOAD_PROC proc = (LOAD_PROC)GetProcAddress(libraryHandle, "LoadAnagameResources");
	if (dlError())
		return false;

	proc();
#endif

	return false;
}
