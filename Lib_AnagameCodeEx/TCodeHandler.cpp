
#include "TCodeHandler.h"
#include <AnafacePage.h>
#include <TWindow.h>
#include <TIdeWindow.h>


void DoLoadFile(TrecPointer<TFileShell> file, TrecPointer<TTextInput> input)
{
	if (!(file.Get() && input.Get()))
		return;

	TFile theFile(file, TFile::t_file_open_always | TFile::t_file_read);
	if (!theFile.IsOpen())
	{
		return;
	}
	TString readInto;
	TString contents;
	while (theFile.ReadString(readInto, 1000ULL))
	{
		contents.Append(readInto);
	}

	input->SetText(contents);
	contents.Empty();
}

void TCodeHandler::LoadCompiler()
{
	if (compiler.Get() || !window.Get() || !file.Get())
		return;
	


	auto basicEnv = window->GetEnvironment();

	TString resource;
	resource.Format(L"%ws:%ws", L"AnagameCodeEx:AG_Compiler:",
		this->file->GetPath().GetConstantBuffer().getBuffer());

	compiler = TrecPointerKey::ConvertPointer<TObject, TcCompiler>( basicEnv->GetResource(resource) );
}

void TCodeHandler::SetDataFile(TrecPointer<TFileShell> file)
{
	TFileEventHandler::SetDataFile(file);
	LoadCompiler();
}

void TCodeHandler::Initialize(TrecPointer<TPage> page)
{
	auto rootPage = TrecPointerKey::ConvertPointer<TPage, AnafacePage>(page);
	//assert(rootPage.Get());
	this->input = TrecPointerKey::ConvertPointer<TControl, TTextInput>(rootPage->GetControlById(L"ActText"));

	DoLoadFile(this->file, this->input);


	// Load Compiler
	TrecPointer<TIdeWindow> window = TrecPointerKey::ConvertPointer<DrawingBoard, TIdeWindow>(page->GetDrawingBoard());

	LoadCompiler();
}

bool TCodeHandler::OnChar(bool fromChar, UINT nChar, UINT nRepCnt, UINT nFlags, message_output* mOut)
{
	return false;
}

void TCodeHandler::SetSelf(TrecPointer<TPage::EventHandler> handleSelf)
{
}

TrecPointer<TPage> TCodeHandler::GetPage()
{
	return TrecPointer<TPage>();
}

void TCodeHandler::HandleEvents(TDataArray<TPage::EventID_Cred>& eventAr)
{
}

void TCodeHandler::SaveFile(TString& ret)
{
	if (!file.Get())
	{
		ret.Set(L"File Not Set");
		return ;
	}
	if (!input.Get())
	{	
		ret.Set( L"File Page not Initialized!");
		return;
	}
	TFile theFile(file, TFile::t_file_open_always | TFile::t_file_truncate_existing | TFile::t_file_write);
	if (!theFile.IsOpen())
	{
		ret.Format(L"Could not open file '%ws'", file->GetPath().GetConstantBuffer().getBuffer());
		return;
	}

	auto str = input->GetTextElement()->GetText();

	//assert(str.Get());

	theFile.WriteString(str->GetString());

	theFile.Close();
}


void TCodeHandler::LoadFile(TString&)
{
	DoLoadFile(this->file, this->input);
}

TrecPointer<TVariable> TCodeHandler::GetData()
{
	return TrecPointer<TVariable>();
}

TString TCodeHandler::SubmitInfo(TrecPointer<TVariable>)
{
	return TString();
}
