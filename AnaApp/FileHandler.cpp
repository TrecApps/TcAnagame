#include "FileHandler.h"
#include <AnafacePage.h>

void FileHandler::Initialize(TrecPointer<TPage> page)
{
	auto handlerPage = TrecPointerKey::ConvertPointer<TPage, AnafacePage>(page);
	nodeControl = TrecPointerKey::ConvertPointer<TPage, TDataNodeControl>(handlerPage->GetRootControl());
	assert(nodeControl.Get());
	PrepNodes();
}

bool FileHandler::OnChar(bool fromChar, UINT nChar, UINT nRepCnt, UINT nFlags, message_output* mOut)
{
	return false;
}

void FileHandler::SetSelf(TrecPointer<TPage::EventHandler> handleSelf)
{
}

TrecPointer<TPage> FileHandler::GetPage()
{
	return TrecPointer<TPage>();
}

void FileHandler::HandleEvents(TDataArray<TPage::EventID_Cred>& eventAr)
{
}

void FileHandler::SetNodes(TrecPointer<TObjectNode> nodes)
{
	this->nodes = nodes;
	PrepNodes();
}

void FileHandler::PrepNodes()
{
	if (nodeControl.Get() && nodes.Get() && !nodeControl->GetNode().Get())
	{
		nodeControl->SetNode(nodes);
	}
}
