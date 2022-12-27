#include "PageTypes.h"

TPageEx::TPageEx(TrecActivePointer<TPage> page, const TString& name) : TPage(page.GetTrecPointer()->GetDrawingBoard())
{
    this->mainPage = page.GetTrecPointer();
    this->name.Set(name);
    this->area = mainPage->GetArea();
}

TrecPointer<TPage> TPageEx::GetPage()
{
    return mainPage;
}

TString TPageEx::GetName()
{
    return name;
}

bool TPageEx::HandlesEvents()
{
    return mainPage->HandlesEvents();
}

void TPageEx::Draw(TrecPointer<TVariable> object)
{
    mainPage->Draw(object);
}

ag_msg void TPageEx::OnRButtonUp(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& cred)
{
    mainPage->OnRButtonUp(nFlags, point, mOut, cred);
}

ag_msg void TPageEx::OnRButtonDown(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& cred)
{
    mainPage->OnRButtonDown(nFlags, point, mOut, cred);
}

ag_msg void TPageEx::OnLButtonUp(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& cred)
{
    mainPage->OnLButtonUp(nFlags, point, mOut, cred);
}
ag_msg void TPageEx::OnLButtonDown(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& cred)
{
    mainPage->OnLButtonDown(nFlags, point, mOut, cred);
}

ag_msg void TPageEx::OnMouseMove(UINT nFlags, TPoint point, message_output& mOut, TDataArray<EventID_Cred>& cred)
{
    mainPage->OnMouseMove(nFlags, point, mOut, cred);
}

ag_msg void TPageEx::OnLButtonDblClk(UINT nFlags, TPoint point, message_output& mOut, TDataArray<EventID_Cred>& eventAr)
{
    mainPage->OnLButtonDblClk(nFlags, point, mOut, eventAr);
}

ag_msg void TPageEx::OnResize(RECT_F& newLoc, UINT nFlags, TDataArray<EventID_Cred>& eventAr)
{
    mainPage->OnResize(newLoc, nFlags, eventAr);
    area = mainPage->GetArea();
}

ag_msg bool TPageEx::OnDestroy()
{
    return mainPage->OnDestroy();
}

ag_msg bool TPageEx::OnScroll(bool fromBars, const TPoint& point, const TPoint& direction, TDataArray<EventID_Cred>& cred)
{
    bool ret = mainPage->OnScroll(fromBars, point, direction, cred);
    area = mainPage->GetArea();
    return ret;
}

TRibbonPage::TRibbonPage(TrecActivePointer<TPage> page, const TString& name, bool isMain) : TPageEx(page, name)
{
    this->isMain = isMain;
}

page_type TRibbonPage::GetPageType()
{
    return isMain ? page_type::main_ribbon : page_type::ribbon_sup;
}

TFilePage::TFilePage(TrecActivePointer<TPage> page, const TString& name, const TString& type): TPageEx(page, name)
{
    TrecPointer<HandlerPage> handlerPage = TrecPointerKey::ConvertPointer<TPage, HandlerPage>(page.GetTrecPointer());
    this->fileEventHandler = TrecPointerKey::ConvertPointer<TPage::EventHandler, TFileEventHandler>(handlerPage->GetHandler());
    this->type.Set(type);
}

page_type TFilePage::GetPageType()
{
    return page_type::file;
}

void TFilePage::SetFile(TrecPointer<TFileShell> file)
{
    if (fileEventHandler.Get())
        fileEventHandler->SetFile(file);
}

TString TFilePage::SaveFile()
{
    if(!fileEventHandler.Get())
        return L"No Handler Set";
    return fileEventHandler->SaveFile();
}

TString TFilePage::LoadFile()
{
    if (!fileEventHandler.Get())   
        return L"No Handler Set";
    return fileEventHandler->LoadFile();
}

TrecPointer<TVariable> TFilePage::GetData()
{
    if(!fileEventHandler.Get())
        return TrecPointer<TVariable>();
    return fileEventHandler->GetData();
}

TString TFilePage::SubmitInfo(TrecPointer<TVariable> obj)
{
    if(!fileEventHandler.Get())
        return  L"No Handler Set";
    return fileEventHandler->SubmitInfo(obj);
}

void TFileEventHandler::SetFile(TrecPointer<TFileShell> file)
{
    this->file = file;
}

TFileSupPage::TFileSupPage(TrecActivePointer<TPage> page, const TString& name): TPageEx(page, name)
{
    TrecPointer<HandlerPage> handlerPage = TrecPointerKey::ConvertPointer<TPage, HandlerPage>(page.GetTrecPointer());
    this->fileSupHandler = TrecPointerKey::ConvertPointer<TPage::EventHandler, TFileSupHandler>(handlerPage->GetHandler());
}

bool TFileSupPage::GetFilePageTypes(TDataArray<TString>& types)
{
    if(!this->fileSupHandler.Get())
        return false;
    fileSupHandler->GetFilePageTypes(types);
}
