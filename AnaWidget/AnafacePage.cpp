#include "TScrollerPage.h"
#include "AnafacePage.h"
#include <TFormatReader.h>
#include <TContainerVariable.h>
#include "TRadioButton.h"
#include "TCheckbox.h"
//#include "TConsoleText.h"
//#include "TConsoleLayout.h"
#include "TArenaView.h"
#include "TVideoControl.h"
#include "TDataLayout.h"
#include "TLayout.h"
#include "TGrid.h"
#include "TDataNodeControl.h"
#include "TTextLayout.h"
//#include "TTextCombobox.h"
#include "TSwitchControl.h"

TString objectTypes[] = {
	L"TControl",		// Basic Control
	L"TRadioButton",	// Control that acts as a radio button
	L"TCheckBox",		// Control that acts as a Checkbox
	L"TInput",			// Control that acts as a basic input element
	L"TLongInput",		// Control that provides more complex input
	L"TBasicPrompt",	// Extends TLongInput to support Command Prompt-like interface
	L"TComplexPrompt",	// Partitions components of a Prompt set-up
	L"TArena",			// Supports a view for 3D content
	L"TVideo",			// Provides support for video playback
	L"TCanvas",			// Allows for drawing custom items
	L"TImage",			// Provides advanged image manipulation options
	L"TComboBox",
	L"TTextComboBox",
	L"TRandomLayout",	// Control that does not organize child layouts
	L"TLayout",			// Control that organizes child layouts in one dimension
	L"TGrid",			// Control that organizes child layouts in a grid
	L"TDataLayout",		// TLayout that adds data-binding
	L"TDataGrid",		// TGrid that supports data-binding
	L"TSwitchControl",	// Layout that presents one child at a time and supports tabbing
	L"TDataNodeControl" // Emulates Trees
};

UINT minLayoutIndex = 10;


void AnafacePage::HandleNode(const TString& name, TString& result, TrecPointer<TVariable> var, TrecPointer<TPage>& curPage, LayoutData& ld)
{
	if (!var.Get() || (var->GetVarType() != var_type::json && var->GetVarType() != var_type::list))
		return;

	TString controlName;
	TrecPointer<TVariable> childVar;

	// We are in a children node, used by File Formats (such as JSON) that does not support Duplicate Key Values and as such are likely in an Array
	if (name.StartsWith(L"children", true))
	{
		for (UINT Rust = 0; Rust < dynamic_cast<TContainerVariable*>(var.Get())->GetSize(); Rust++)
		{
			dynamic_cast<TContainerVariable*>(var.Get())->GetValueAt(Rust, childVar);
			if (!childVar.Get())
				continue;

			HandleNode(L"", result, childVar, curPage, ld);
			if (result.GetSize())
				return;
		}

		return;

	}

	// This a Top Node,
	if (!name.GetSize() && dynamic_cast<TJsonVariable*>(var.Get()))
	{
		TString title;
		for (UINT Rust = 0; dynamic_cast<TJsonVariable*>(var.Get())->RetrieveFieldAt(Rust, title, childVar); Rust++)
		{
			for (UINT C = 0; C < ARRAYSIZE(objectTypes); C++)
			{
				if (title.StartsWith(objectTypes[C], true))
				{
					controlName.Set(title);

				}
			}
		}
	}
	else controlName.Set(name);

	auto page = HandleControl(controlName, result, childVar, ld);

	if (curPage.Get())
	{
		if (ld.isLayout)
		{
			dynamic_cast<TRandomLayout*>(curPage.Get())->AddPage(page, ld.row, ld.col);
		}
		else if (!dynamic_cast<TControl*>(curPage.Get()) || !dynamic_cast<TControl*>(curPage.Get())->InjectChildTemplate(page))
			result.Format(L"Control type %ws does not support Child Controls!", name.SubString(0, name.Find(L'(')).GetConstantBuffer().getBuffer());
	}
	else if (!result.GetSize())
		curPage = page;
}

TrecPointer<TPage> AnafacePage::HandleControl(const TString& name, TString& result, TrecPointer<TVariable> var, LayoutData& ld)
{
	TrecPointer<TPage> ret;

	LayoutData curLd{ false, -1,-1,-1,-1 };

	if (name.StartsWith(L"TRadioButton", true))
		ret = TrecPointerKey::GetNewSelfTrecPointerAlt<TPage, TRadioButton>(this->drawingBoard, styles);
	if (name.StartsWith(L"TCheckBox", true))
		ret = TrecPointerKey::GetNewSelfTrecPointerAlt<TPage, TCheckBox>(this->drawingBoard, styles);
	if (name.StartsWith(L"TInput", true))
		ret = TrecPointerKey::GetNewSelfTrecPointerAlt<TPage, TTextInput>(this->drawingBoard, styles);
	if (name.StartsWith(L"TLongInput", true))
		ret = TrecPointerKey::GetNewSelfTrecPointerAlt<TPage, TTextLayout>(this->drawingBoard, styles);
	//if (name.StartsWith(L"TBasicPrompt", true))
	//	ret = TrecPointerKey::GetNewSelfTrecPointerAlt<TPage, TConsoleText>(this->drawingBoard, styles);
	//if (name.StartsWith(L"TComplexPrompt", true))
	//	ret = TrecPointerKey::GetNewSelfTrecPointerAlt<TPage, TConsoleLayout>(this->drawingBoard, styles);
	if (name.StartsWith(L"TArena", true))
		ret = TrecPointerKey::GetNewSelfTrecPointerAlt<TPage, TArenaView>(this->drawingBoard, styles);
	if (name.StartsWith(L"TVideo", true))
		ret = TrecPointerKey::GetNewSelfTrecPointerAlt<TPage, TVideoControl>(this->drawingBoard, styles);
	//if (name.StartsWith(L"TCanvas", true))
	//	ret = TrecPointerKey::GetNewSelfTrecPointerAlt<TPage, TRadiobutton>(this->drawingBoard, styles);
	//if (name.StartsWith(L"TImage", true))
	//	ret = TrecPointerKey::GetNewSelfTrecPointerAlt<TPage, TRadiobutton>(this->drawingBoard, styles);

	//if (name.StartsWith(L"TComboBox", true))
	//	ret = TrecPointerKey::GetNewSelfTrecPointerAlt<TPage, TCombobox>(this->drawingBoard, styles);
	//if (name.StartsWith(L"TTextComboBox", true))
	//	ret = TrecPointerKey::GetNewSelfTrecPointerAlt<TPage, TTextCombobox>(this->drawingBoard, styles, this->drawingBoard->GetWindowHandle());

	if (name.StartsWith(L"TDataNodeControl", true))
	{
		ret = TrecPointerKey::GetNewSelfTrecPointerAlt<TPage, TDataNodeControl>(this->drawingBoard, styles);
		curLd.isLayout = true;
	}
	if (name.StartsWith(L"TRandomLayout", true))
	{
		ret = TrecPointerKey::GetNewSelfTrecPointerAlt<TPage, TRandomLayout>(this->drawingBoard, styles);
		curLd.isLayout = true;
	}
	if (name.StartsWith(L"TLayout", true))
	{
		ret = TrecPointerKey::GetNewSelfTrecPointerAlt<TPage, TLayout>(this->drawingBoard, styles);
		curLd.isLayout = true;
	}
	if (name.StartsWith(L"TGrid", true))
	{
		ret = TrecPointerKey::GetNewSelfTrecPointerAlt<TPage, TGrid>(this->drawingBoard, styles);
		curLd.isLayout = true;
	}
	if (name.StartsWith(L"TDataLayout", true))
	{
		ret = TrecPointerKey::GetNewSelfTrecPointerAlt<TPage, TDataLayout>(this->drawingBoard, styles);
		curLd.isLayout = true;
	}
	if (name.StartsWith(L"TSwitchControl"))
	{
		ret = TrecPointerKey::GetNewSelfTrecPointerAlt<TPage, TSwitchControl>(this->drawingBoard, styles);
		curLd.isLayout = true;
	}

	// Just a Basic TControl
	if (name.StartsWith(L"TControl", true))
		ret = TrecPointerKey::GetNewSelfTrecPointerAlt<TPage, TControl>(this->drawingBoard, styles);

	if (!ret.Get())
	{
		result.Format(L"Unsupported Control Name %ws detected!", name.GetConstantBuffer().getBuffer());
		return ret;
	}

	if (!dynamic_cast<TJsonVariable*>(var.Get())) return ret;

	TString attName;
	TrecPointer<TVariable> chVar;

	bool metaFound = false;
	for (UINT Rust = 0; dynamic_cast<TJsonVariable*>(var.Get())->RetrieveFieldAt(Rust, attName, chVar); Rust++)
	{
		// In the Event that Our format uses a _metadata attribute to hold other metadata (like XML)
		if (attName.StartsWith(L"_metadata"))
		{
			HandleAttributes(result, ret, chVar, ld);
			metaFound = true;
		}
		if (result.GetSize())
		{
			ret.Nullify();
			return ret;
		}
	}

	if (!metaFound)
		HandleAttributes(result, ret, var, ld);


	for (UINT Rust = 0; dynamic_cast<TJsonVariable*>(var.Get())->RetrieveFieldAt(Rust, attName, chVar); Rust++)
	{
		if (result.GetSize())
		{
			ret.Nullify();
			return ret;
		}
		// Check to see if we have a child Attribute
		if (attName.StartsWith(L"children", true))
		{
			if (curLd.isLayout || (dynamic_cast<TControl*>(ret.Get()) && dynamic_cast<TControl*>(ret.Get())->SupportsChildTemplateInjection()))
				HandleNode(attName, result, chVar, ret, curLd);
			else
				result.Format(L"Control type %ws does not support Child Controls!", name.SubString(0, name.Find(L'(')).GetConstantBuffer().getBuffer());
			continue;
		}

		for (UINT C = 0; C < ARRAYSIZE(objectTypes); C++)
		{
			if (attName.StartsWith(objectTypes[C], true))
			{
				if (curLd.isLayout)
				{
					auto p = HandleControl(attName, result, chVar, curLd);

					if (p.Get() && !result.GetSize())
						dynamic_cast<TRandomLayout*>(ret.Get())->AddPage(p, curLd.row, curLd.col);
				}
				else if (dynamic_cast<TControl*>(ret.Get()) && dynamic_cast<TControl*>(ret.Get())->SupportsChildTemplateInjection())
				{
					auto p = HandleControl(attName, result, chVar, curLd);

					if (p.Get() && !result.GetSize())
						dynamic_cast<TControl*>(ret.Get())->InjectChildTemplate(p);
				}
				else
					result.Format(L"Control type %ws does not support Child Controls!", name.SubString(0, name.Find(L'(')).GetConstantBuffer().getBuffer());
				if (result.GetSize())
				{
					ret.Nullify();
					return ret;
				}
				continue;
			}
		}
	}

	if (result.GetSize())
	{
		ret.Nullify();
		return ret;
	}

	int parIndex = name.Find(L'(');

	if (result.GetSize())
		ret.Nullify();
	else if (ld.isLayout && parIndex != -1)
	{
		TString details(name.SubString(parIndex + 1, name.Find(L')')));

		auto splitDetails = details.splitn(L',', 4);

		switch (splitDetails->Size())
		{
		case 4:
			splitDetails->at(3).ConvertToInt(ld.endCol);
		case 3:
			splitDetails->at(2).ConvertToInt(ld.endRow);
		case 2:
			splitDetails->at(1).ConvertToInt(ld.col);
		case 1:
			splitDetails->at(0).ConvertToInt(ld.row);
		}
	}
	return ret;
}

void AnafacePage::HandleAttributes(TString& result, TrecPointer<TPage>& curPage, TrecPointer<TVariable> var, LayoutData& ld)
{
	TString attName;
	TrecPointer<TVariable> chVar;
	if (!dynamic_cast<TJsonVariable*>(var.Get()))return;
	for (UINT Rust = 0; dynamic_cast<TJsonVariable*>(var.Get())->RetrieveFieldAt(Rust, attName, chVar); Rust++)
	{
		if (result.GetSize()) return;
		if (!chVar.Get())
			continue;

		auto strVar = TrecPointerKey::ConvertPointer<TVariable, TStringVariable>(chVar->ToString());
		if (!strVar.Get())
			continue;

		if (!attName.CompareNoCase(L"IsGallery") && (chVar->Get4Value() || strVar->GetString().GetTrim().CompareNoCase(L"true")))
		{
			if (dynamic_cast<TLayout*>(curPage.Get()))
				dynamic_cast<TLayout*>(curPage.Get())->primaryStack = false;
		}

		if (!attName.CompareNoCase(L"RowHeights"))
		{
			HandleDimensionList(true, result, curPage, chVar);
		}
		else if (!attName.CompareNoCase(L"ColumnWidths"))
		{
			HandleDimensionList(false, result, curPage, chVar);
		}
		else if (!attName.CompareNoCase(L"RowHeight") || !attName.CompareNoCase(L"ColumnWidth"))
		{
			bool row = !attName.CompareNoCase(L"RowHeight");
			TString data(strVar->GetString());

			bool isFlex = false;
			if (data.EndsWith(L'*'))
			{
				isFlex = true;
				data.Delete(data.GetSize() - 1);
			}

			UINT uValue = 0;
			if (!TString::ConvertStringToUint(data, uValue))
			{
				result.Format(L"Failed to parse number for %ws attribute!", row ? L"Row Height" : L"Column Width");
				return;
			}
			row ? dynamic_cast<TLayout*>(curPage.Get())->AddRow(uValue, isFlex) :
				dynamic_cast<TLayout*>(curPage.Get())->AddCol(uValue, isFlex);
		}
		else if (!attName.CompareNoCase(L"RowPosition"))
		{
			auto details = strVar->GetString().splitn(L',', 2);
			details->at(0).ConvertToInt(ld.row);
			if (details->Size() > 1)
				details->at(1).ConvertToInt(ld.endRow);
		}
		else if (!attName.CompareNoCase(L"ColPosition") || !attName.CompareNoCase(L"ColumnPosition"))
		{
			auto details = strVar->GetString().splitn(L',', 2);
			details->at(0).ConvertToInt(ld.col);
			if (details->Size() > 1)
				details->at(1).ConvertToInt(ld.endCol);
		}
		else
		{
			dynamic_cast<TControl*>(curPage.Get())->AddAttribute(attName, strVar->GetString());
		}
	}
}

void AnafacePage::HandleDimensionList(bool row, TString& result, TrecPointer<TPage>& curPage, TrecPointer<TVariable> var)
{
	if (!dynamic_cast<TLayout*>(curPage.Get()) && !dynamic_cast<TContainerVariable*>(var.Get()))
		return;

	for (UINT Rust = 0; Rust < dynamic_cast<TContainerVariable*>(var.Get())->GetSize(); Rust++)
	{
		TrecPointer<TVariable> value;
		dynamic_cast<TContainerVariable*>(var.Get())->GetValueAt(Rust, value);
		if (!value.Get())
		{
			result.Format(L"Failed to parse number for %ws attribute!", row ? L"Row Height" : L"Column Width");
			return;
		}
		auto strVar = TrecPointerKey::ConvertPointer<TVariable, TStringVariable>(value->ToString());
		if (!dynamic_cast<TStringVariable*>(strVar.Get()))
		{
			result.Format(L"Failed to parse number for %ws attribute!", row ? L"Row Height" : L"Column Width");
			return;
		}
		TString data(strVar->GetString());

		bool isFlex = false;
		if (data.EndsWith(L'*'))
		{
			isFlex = true;
			data.Delete(data.GetSize() - 1);
		}

		UINT uValue = 0;
		if (!TString::ConvertStringToUint(data, uValue))
		{
			result.Format(L"Failed to parse number for %ws attribute!", row ? L"Row Height" : L"Column Width");
			return;
		}

		row ? dynamic_cast<TLayout*>(curPage.Get())->AddRow(uValue, isFlex) :
			dynamic_cast<TLayout*>(curPage.Get())->AddCol(uValue, isFlex);
	}

}

AnafacePage::AnafacePage(TrecPointer<DrawingBoard> board, TrecPointer<EventHandler> handler) : HandlerPage(board, handler)
{
}

bool AnafacePage::HandlesEvents()
{
	return handler.Get() != nullptr;
}

void AnafacePage::Create(const RECT_F& loc)
{
	if (!dynamic_cast<TControl*>(rootPage.Get()))
		return;
	dynamic_cast<TControl*>(rootPage.Get())->onCreate(loc, directory);
	area = rootPage->GetArea();
}

TString AnafacePage::PrepPage(TrecPointer<TFileShell> file, TrecPointer<EventHandler> handler)
{
	TString ret;
	if (!file.Get())
		ret.Set(L"Null File");
	if (!handler.Get())
	{
		if (ret.GetSize())
			ret.Append(L" And ");
		ret.Append(L"Null Handler");
	}

	this->handler = handler;

	if (ret.GetSize())
	{
		ret.Append(L"provided!");
		return ret;
	}

	TrecPointer<TFormatReader> reader = TFormatReader::GetReader(file);

	if (!reader.Get())
	{
		ret.Format(L"Could Not get a format reader for '%ws'. Likely because File format is not supported!", file->GetName().GetConstantBuffer().getBuffer());
		return ret;
	}

	ret.Set(reader->Read());

	if (ret.GetSize())
		return ret;

	TrecPointer<TVariable> uiData = reader->GetData();

	LayoutData ld{ false, -1,-1,-1,-1 };

	HandleNode(L"", ret, uiData, rootPage, ld);

	if (!ret.GetSize() && this->handler.Get())
		handler->Initialize(TrecPointerKey::TrecFromSoft<>(self));

	directory = file->GetParent();

	return ret;
}

void AnafacePage::Draw(TrecPointer<TVariable> object)
{
	if (rootPage.Get())
		rootPage->Draw(object);
}

void AnafacePage::OnRButtonUp(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& cred)
{
	if (rootPage.Get())
		rootPage->OnRButtonUp(nFlags, point, mOut, cred);
	if (mOut != message_output::mo_negative)
	{
		if (handler.Get())
			handler->HandleEvents(cred);

		mOut = message_output::mo_positive_handled;
	}
}

void AnafacePage::OnRButtonDown(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& eventAr)
{
	if (rootPage.Get())
		rootPage->OnRButtonDown(nFlags, point, mOut, eventAr);
	if (mOut != message_output::mo_negative)
	{
		if (handler.Get())
			handler->HandleEvents(eventAr);

		mOut = message_output::mo_positive_handled;
	}
}

void AnafacePage::OnLButtonUp(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& eventAr)
{
	if (rootPage.Get())
		rootPage->OnLButtonUp(nFlags, point, mOut, eventAr);
	if (mOut != message_output::mo_negative)
	{
		if (handler.Get())
			handler->HandleEvents(eventAr);

		mOut = message_output::mo_positive_handled;
	}
}

void AnafacePage::OnLButtonDown(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& eventAr)
{
	if (rootPage.Get())
		rootPage->OnLButtonDown(nFlags, point, mOut, eventAr);
	if (mOut != message_output::mo_negative)
	{
		if (handler.Get())
			handler->HandleEvents(eventAr);

		mOut = message_output::mo_positive_handled;
	}
}

void AnafacePage::OnMouseMove(UINT nFlags, TPoint point, message_output& mOut, TDataArray<EventID_Cred>& eventAr)
{
	if (rootPage.Get())
		rootPage->OnMouseMove(nFlags, point, mOut, eventAr);
	if (mOut != message_output::mo_negative)
	{
		if (handler.Get())
			handler->HandleEvents(eventAr);

		mOut = message_output::mo_positive_handled;
	}
}

void AnafacePage::OnLButtonDblClk(UINT nFlags, TPoint point, message_output& mOut, TDataArray<EventID_Cred>& events)
{
	if (rootPage.Get())
		rootPage->OnLButtonDblClk(nFlags, point, mOut, events);
	if (mOut != message_output::mo_negative)
	{
		if (handler.Get())
			handler->HandleEvents(events);

		mOut = message_output::mo_positive_handled;
	}
}

void AnafacePage::OnResize(RECT_F& newLoc, UINT nFlags, TDataArray<EventID_Cred>& eventAr)
{
	if (rootPage.Get())
		rootPage->OnResize(newLoc, nFlags, eventAr);
	if (handler.Get())
		handler->HandleEvents(eventAr);

	// We are handling events by Child element, so no need to keep them
	eventAr.RemoveAll();
	this->area = newLoc;
}

bool AnafacePage::OnDestroy()
{
	return true;
}

bool AnafacePage::OnScroll(bool, const TPoint& point, const TPoint& direction, TDataArray<EventID_Cred>&)
{
	return false;
}

void AnafacePage::InjectScrollerPage(const RECT_F& bounds, const RECT_F& needs, TrecPointer<TPage> page)
{
	if (page.Get() == rootPage.Get())
	{
		TrecPointer<TPage> scroller = TrecPointerKey::GetNewSelfTrecPointerAlt<TPage, TScrollerPage>(drawingBoard, page);
		TDataArray<EventID_Cred> cred;
		auto b = bounds;
		scroller->OnResize(b, 0, cred);
		rootPage = scroller;
	}
}

TrecPointer<TPage> AnafacePage::GetRootControl()
{
	return rootPage;
}

TrecPointer<TPage::EventHandler> AnafacePage::GetHandler()
{
	return handler;
}

TrecPointer<TControl> AnafacePage::GetControlById(const TString& id)
{
	TrecPointer<TControl> cMainPage = TrecPointerKey::ConvertPointer < TPage, TControl>(rootPage);

	return cMainPage.Get() ? cMainPage->GetControlById(id) : TrecPointer<TControl>();
}
