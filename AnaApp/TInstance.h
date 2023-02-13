#pragma once
#include <TString.h>
#include "TWindow.h"
#include <GLFW/glfw3.h>
#include "AnaApp.h"
#include <TEnvironment.h>

using t_window_type = enum class t_window_type
{
    t_window_type_plain,  // Gets a Plain Window
    t_window_type_ide,    // Gets a Basic IDE style Window
    t_window_type_web     // Window Optimized for Web-Browsing
};

using t_dialog_modal_mode = enum class t_dialog_modal_mode {

    // If you intend to get a result while using 'no_modal' or 'soft_modal' you should set a callback runner on the procedure to handle it
    no_modal,   // Parent Window continues to operate while Child Window is open
    soft_modal, // Parent window can still move around and be resized, but is otherwise disabled while Child Window is open
    hard_modal  // Parent Window is utterly disabled while the child window is open
};

using LibHandle =
#ifdef _WINDOWS
HMODULE
#elif defined(__linux__) || (defined (__APPLE__) && defined (__MACH__))
void*
#endif
;


class _ANA_APP TLibrary {
    TString name;
    TString builderName;                        // The name of the Environment Builder provided (Empty if not available)
    TDataArray<TString> environmentNames;       // Names of the Environments available
    TDataArray<TString> projectFiles;           // If a certain file name is located in a given directory, This Library holds an environment that can read it into a project 
    TDataArray<TString> projectFileExtensions;  // If a certain File Type is located in a given directory, This Library holds an environment that can read it into a project
    LibHandle libraryHandle;                    // Reference to the library. If it is 0/null, this library has NOT been loaded

    bool ScanProjectFiles(TrecPointer<TFileShell> file);
    bool ScanProjectExts(TrecPointer<TFileShell> file);
public:
    TLibrary();
    TLibrary(const TLibrary& copy) = default;
    void SetName(const TString& name);
    TString GetName();
    void SetBuilderName(const TString& bName);
    TString GetBuilderName();
    void AppendProjectType(const TString& pName);
    bool GetProjectAt(UINT c, TString& name);
    void AppendProjectFile(const TString& fileName);
    void AppendProjectExt(const TString& fileExt);
    bool SupportsProjectDirectory(TrecPointer<TFileShell> directory);

    bool IsLoaded();
    bool Load(TrecPointer<TFileShell> directory);
};

class _ANA_APP TProjectData
{
public:
    mutable TrecPointer<TDirectory> directory;
    TString projectName;
    TString builderName;
    TString environmentName;

    bool HasDirectory() const;
};

class _ANA_APP TInstance :
    public TVObject
{
    friend class TrecPointerKey;
    friend class TWindow;

    int glfwInitResult;
    TDataArray<TrecPointer<TWindow>> windows;

    GLFWcharfun charFunction;
    GLFWcursorposfun cursorFunction;
    GLFWkeyfun keyFunction;
    GLFWmousebuttonfun mouseFunction;
    GLFWscrollfun scrollFunction;
    GLFWwindowfocusfun focusFunction;
    GLFWwindowsizefun resizeFunction;

    GLFWwindowclosefun closeFunction;

    TrecPointer<TWindow> GetWindow(GLFWwindow* win);

    UINT windowCount;

    TrecPointerSoft<TInstance> self;
    TInstance();

    // Help Manage Extension Libraries available to Anagame
    void ReadLibraryList();
    TrecPointer<TFileShell> libraryFolder;
    TDataArray<TLibrary> libraryList;

    TDataArray<TrecPointer<TEnvironmentBuilder>> environmentBuilders;


    void RemoveWindow(GLFWwindow* window);

public:
    static TrecPointer<TInstance> GetInstance();
    ~TInstance();

    void SubmitEnvironmentBuilder(TrecPointer<TEnvironmentBuilder> builder);

    TrecPointer<TVariable> GetAvailableProjectTypes();

    TrecPointer<TVariable> GetExisitngProjects();
    TString SaveProject(const TProjectData& project);

    /**
     * Will Scan for a given resource. If 'env' is provided, then it will first check to see if the environment will provide it.
     * If 'env' is not provided, or it does not provide the resource, The Instance scans for a list of Registered Builders that can support the resource
     * 
     * If a Builder is found, then true is returned. If 'env' is provided then the Builder builds a new environment, which is added to 'env'
     */
    bool ScanForResource(const TString& resource, TrecPointer<TEnvironment> env);

    /**
     * Causes Anagame to load the requested Set of Libraries. In the future, if a library is not currently listed, then it will reach out to a repository for the requested Library
     * For now, any missing library will imply be added to the return value.
     * 
     * An Empty list returned indicates that all requested Libraries were successfully loaded.
     */
    TDataArray<TString> LoadLibraries(const TDataArray<TString>& libraries);

    void SetSelf(TrecPointer<TInstance> newSelf);

    void DoDraw();

    int GetGlfwInitResult();

    UINT HasWindows();

    UINT GenerateWindow(TrecPointer<TWindow>& window, TrecPointer<TFileShell> uiInterface, const TString& name, t_window_type type = t_window_type::t_window_type_plain);
    UINT GenerateDialog(TrecPointer<TWindow>& window, TrecPointer<TWindow> parent,
        const TString& name, TrecPointer<TPageBuilder> page, 
        TrecPointer<TFileShell> uiInterface, const TString& details,
        t_dialog_modal_mode modalMode);

    void SetCallbacks(GLFWcharfun, GLFWcursorposfun, GLFWkeyfun, GLFWmousebuttonfun, GLFWscrollfun, GLFWwindowfocusfun, GLFWwindowsizefun, GLFWwindowclosefun);

    void OnChar(GLFWwindow* win, UINT ch);
    void OnMouseMove(GLFWwindow* window, double xpos, double ypos);
    void OnLButtonDown(GLFWwindow* window, int mods);
    void OnLButtonUp(GLFWwindow* window, int mods);
    void OnRButtonDown(GLFWwindow* window, int mods);
    void OnRButtonUp(GLFWwindow* window, int mods);
    void OnScroll(GLFWwindow* window, double x, double y);
    void OnFocus(GLFWwindow* window);
    void OnLoseFocus(GLFWwindow* window);
    void OnWindowResize(GLFWwindow* window, int w, int h);
    void OnWindowClose(GLFWwindow* window);
    void OnKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods);
};

