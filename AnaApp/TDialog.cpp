#include "TDialog.h"

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


	dialogRunning = false;
	return true;
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
