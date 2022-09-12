#include <TInstance.h>

TrecPointer<TInstance> mainInstance;

int main()
{

	mainInstance = TrecPointerKey::GetNewTrecPointer<TInstance>();
	if (!mainInstance->GetGlfwInitResult())
		return -1;

	TrecPointer<TWindow> mainWindow;

	mainInstance->GenerateWindow(mainWindow, TrecPointer<TFileShell>(), L"");

	if (!mainWindow.Get())
	{
		mainInstance.Nullify();
		return -2;
	}

	

	return 0;
}