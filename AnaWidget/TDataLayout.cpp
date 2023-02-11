#include "TDataLayout.h"

#include <TContainerVariable.h>

TDataLayout::TDataLayout(TrecPointer<DrawingBoard> drawingBoard, TDataMap<TDataMap<TString>> styles) : TLayout(drawingBoard, styles)
{
	width = 120;
	height = 60;
	stackFirst = true;
	maxPrimaryCount = -1;
	org = data_org::do_default;

	LayoutSpace space;
	space.space = 1;
	space.isFlex = true;

	primDem.push_back(space);
}

void TDataLayout::SetData(TrecPointer<TVariable> var)
{
	this->var = var;
}

void TDataLayout::Draw(TrecPointer<TVariable> object)
{
	TControl::Draw(object);

	auto tempRec = area;
	tempRec.bottom = tempRec.top + height;
	if(width)
		tempRec.right = tempRec.left + width;

	TrecPointer<TPage> tempPage = childControls.Size() ? childControls.at(0).control : TrecPointer<TPage>();
	if (!tempPage.Get())return;

	TDataArray<EventID_Cred> cred;
	TrecPointer<TVariable> varValue;
	tempPage->OnResize(tempRec, 0, cred);

	if (dynamic_cast<TContainerVariable*>(var.Get()))
	{
		switch (org)
		{
		case data_org::do_default:
			if (stackFirst)
			{
				for (UINT Rust = 0; dynamic_cast<TContainerVariable*>(var.Get())->GetValueAt(Rust, varValue); Rust++)
				{

					//if (dynamic_cast<TContainerVariable*>(varValue.Get()))
					//{
					//	TrecPointer<TVariable> varValue1;
					//	for (UINT C = 0; dynamic_cast<TContainerVariable*>(varValue.Get())->GetValueAt(C, varValue1); C++)
					//	{
					//		tempPage->Draw(varValue1);
					//		tempRec.left += width;
					//		tempRec.right += width;
					//		tempPage->OnResize(tempRec, 0, cred);
					//	}
					//	tempRec.left = area.left;
					//	tempRec.right = tempRec.left + width;
					//}
					//else
						tempPage->Draw(varValue);
					tempRec.bottom += height;
					tempRec.top += height;
					tempPage->OnResize(tempRec, 0, cred);
				}
			}
			else
			{
				for (UINT Rust = 0; dynamic_cast<TContainerVariable*>(var.Get())->GetValueAt(Rust, varValue); Rust++)
				{

					if (dynamic_cast<TContainerVariable*>(varValue.Get()))
					{
						TrecPointer<TVariable> varValue1;
						for (UINT C = 0; dynamic_cast<TContainerVariable*>(varValue.Get())->GetValueAt(C, varValue1); C++)
						{
							tempPage->Draw(varValue1);
							tempRec.top += height;
							tempRec.bottom += height;
							tempPage->OnResize(tempRec, 0, cred);
						}
						tempRec.top = area.top;
						tempRec.bottom = tempRec.top + height;
					}
					else
						tempPage->Draw(varValue);
					tempRec.left += width;
					tempRec.right += width;
				}
			}
			break;
		case data_org::do_limit_by_space:
			for (UINT Rust = 0; dynamic_cast<TContainerVariable*>(var.Get())->GetValueAt(Rust, varValue); Rust++)
			{
				tempPage->Draw(varValue);

				if (stackFirst)
				{
					tempRec.top += height;
					tempRec.bottom += height;
					if (tempRec.bottom > area.bottom)
					{
						tempRec.top = area.top;
						tempRec.bottom = tempRec.top + height;
						tempRec.left += width;
						tempRec.right += width;
					}
				}
				else
				{
					tempRec.left += width;
					tempRec.right += width;
					if (tempRec.right > area.right)
					{
						tempRec.left = area.left;
						tempRec.right = tempRec.left + width;
						tempRec.top += height;
						tempRec.bottom += height;
					}
				}
			}


			break;
		}
	}
	/*else
		tempPage->Draw(var);*/
}

bool TDataLayout::onCreate(const RECT_F& loc, TrecPointer<TFileShell> d)
{
	TString valpoint;

	if (attributes.retrieveEntry("ColumnWidth", valpoint))
	{
		int value = 0;
		if (!valpoint.ConvertToInt(value))
		{
			width = value;
		}
	}

	;
	if (attributes.retrieveEntry(L"RowHeight", valpoint))
	{
		int value = 0;
		if (!valpoint.ConvertToInt(value))
		{
			height = value;
		}
	}

	if (attributes.retrieveEntry(L"DefaultVertical", valpoint))
	{
		if (!valpoint.CompareNoCase(L"false") || !valpoint.CompareNoCase(L'0'))
		{
			this->stackFirst = false;
		}
	}

	if (attributes.retrieveEntry(L"LimitPrimaryDimension", valpoint))
	{
		if (!valpoint.CompareNoCase(L"true") || valpoint.CompareNoCase(L'0'))
		{
			org = data_org::do_limit_by_space;
		}
	}




	//if (attributes.retrieveEntry(L"MaxItemCountPrim", valpoint))
	//{
	//	if (!valpoint.ConvertToInt(this->maxPrimaryCount))
	//	{
	//		
	//	}
	//	else
	//	{
	//		maxPrimaryCount = -1;
	//	}
	//}

	return TRandomLayout::onCreate(loc, d);
}

RECT_F TDataLayout::GetArea()
{
	switch (org)
	{
	case data_org::do_default:
		return GetAreaDefault();
	case data_org::do_limit_by_item_count:
		return GetAreaByCount();
	default:
		return GetAreaByContainer();
	}

}

void TDataLayout::OnRButtonUp(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& cred)
{
	bool isCurClick = this->isRightClicked;
	TControl::OnRButtonUp(nFlags, point, mOut, cred);

	if (mOut == message_output::mo_negative)
		return;

	TString index = HasEvent(isCurClick ? R_Message_Type::On_Right_Click : R_Message_Type::On_R_Button_Up);
	if (index.GetSize())
	{
		int r = 0; int c = 0;
		int iindex = -1;
		if (GetIndex(point, r, c))
		{
			iindex = ConvertCoordinates(r, c);
		}
		cred.at(cred.Size() - 1).args->arrayLabel = iindex;
	}
}

void TDataLayout::OnRButtonDown(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& cred)
{
	TControl::OnRButtonDown(nFlags, point, mOut, cred);

	if (mOut == message_output::mo_negative)
		return;

	TString index = HasEvent(R_Message_Type::On_R_Button_Down);
	if (index.GetSize())
	{
		int r = 0; int c = 0;
		int iindex = -1;
		if (GetIndex(point, r, c))
		{
			iindex = ConvertCoordinates(r, c);
		}
		cred.at(cred.Size() - 1).args->arrayLabel = iindex;
	}
}

void TDataLayout::OnLButtonUp(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& eventAr)
{
	bool isCurClick = this->isLeftClicked;
	TControl::OnLButtonUp(nFlags, point, mOut, eventAr);

	if (mOut == message_output::mo_negative)
		return;

	TString index(HasEvent(isCurClick ? R_Message_Type::On_Click : R_Message_Type::On_L_Button_Up));
	if (index.GetSize())
	{
		int r = 0; int c = 0;
		int iindex = -1;
		if (GetIndex(point, r, c))
		{
			iindex = ConvertCoordinates(r, c);

			if (isCurClick && 
				dynamic_cast<TContainerVariable*>(var.Get()) &&
				eventAr.Size() &&
				eventAr[eventAr.Size() -1].eventType == R_Message_Type::On_Click &&
				eventAr[eventAr.Size() -1].args.Get() &&
				!eventAr[eventAr.Size() -1].args->methodID.Compare(index))
			{
				EventID_Cred& cred = eventAr[eventAr.Size() -1];
				dynamic_cast<TContainerVariable*>(this->var.Get())->GetValueAt(iindex, cred.data);
				cred.args->arrayLabel = iindex;
			}
		}
	}

	if (isCurClick)
	{
		index.Set(HasEvent(R_Message_Type::On_sel_change));
		if (index.GetSize())
		{
			int r = 0; int c = 0;
			int iindex = -1;
			if (GetIndex(point, r, c))
			{
				iindex = ConvertCoordinates(r, c);

				this->args.Reset();
				this->args.arrayLabel = iindex;
				this->args.eventType = R_Message_Type::On_sel_change;
				this->args.isClick = true;
				this->args.isLeftClick = true;
				this->args.methodID = index;
				this->args.point = point;
				this->args.positive = true;
				this->args.type = L'\0';
				EventID_Cred cred(R_Message_Type::On_sel_change, TrecPointerKey::TrecFromSoft<>(self));
				cred.args = TrecPointerKey::GetNewTrecPointer<EventArgs>(this->args);
				if (dynamic_cast<TContainerVariable*>(this->var.Get()))
					dynamic_cast<TContainerVariable*>(this->var.Get())->GetValueAt(iindex, cred.data);

				eventAr.push_back(cred);
			}
		}
	}
}

void TDataLayout::OnLButtonDown(UINT nFlags, const TPoint& point, message_output& mOut, TDataArray<EventID_Cred>& cred)
{
	TControl::OnLButtonDown(nFlags, point, mOut, cred);

	if (mOut == message_output::mo_negative)
		return;

	TString index = HasEvent(R_Message_Type::On_L_Button_Down);
	if (index.GetSize())
	{
		int r = 0; int c = 0;
		int iindex = -1;
		if (GetIndex(point, r, c))
		{
			iindex = ConvertCoordinates(r, c);
		}
		cred.at(cred.Size() - 1).args->arrayLabel = iindex;
	}
}

void TDataLayout::OnMouseMove(UINT nFlags, TPoint point, message_output& mOut, TDataArray<EventID_Cred>& cred)
{
	bool isCurClick = this->isMouseIn;
	TControl::OnMouseMove(nFlags, point, mOut, cred);

	if (mOut == message_output::mo_negative)
		return;

	TString index = HasEvent(isCurClick ? R_Message_Type::On_Hover : R_Message_Type::On_Hover_Enter);
	if (index.GetSize())
	{
		int r = 0; int c = 0;
		int iindex = -1;
		if (GetIndex(point, r, c))
		{
			iindex = ConvertCoordinates(r, c);
		}
		cred.at(cred.Size() - 1).args->arrayLabel = iindex;
	}
}

void TDataLayout::OnLButtonDblClk(UINT nFlags, TPoint point, message_output& mOut, TDataArray<EventID_Cred>& args)
{
	TControl::OnLButtonDblClk(nFlags, point, mOut, args);

	if (mOut == message_output::mo_negative)
		return;

	TString index = HasEvent(R_Message_Type::On_LDoubleClick);
	if (index.GetSize())
	{
		int r = 0; int c = 0;
		int iindex = -1;
		if (GetIndex(point, r, c))
		{
			iindex = ConvertCoordinates(r, c);
		}
		args.at(args.Size() - 1).args->arrayLabel = iindex;
	}
}

void TDataLayout::OnResize(RECT_F& newLoc, UINT nFlags, TDataArray<EventID_Cred>& eventAr)
{
	bounds = newLoc;
	auto curLoc = GetArea();

	if (curLoc.left > bounds.left)
	{
		OnScroll(false, TPoint(), TPoint(curLoc.left - bounds.left, 0.0f), eventAr);
	}
	if (curLoc.top > bounds.top)
	{
		OnScroll(false, TPoint(), TPoint(0.0f, curLoc.top - bounds.top), eventAr);
	}

	bounds = newLoc;

	if (curLoc.right > bounds.right || curLoc.bottom > bounds.bottom)
	{
		auto actParent = TrecPointerKey::TrecFromSoft<>(parent);
		if (actParent.Get())
			actParent->InjectScrollerPage(bounds, curLoc, TrecPointerKey::TrecFromSoft<>(self));
	}
	area = curLoc;
}

bool TDataLayout::InjectChildTemplate(TrecPointer<TPage> page)
{
	childControls.RemoveAll();
	if (!page.Get())
		return false;
	ChildControl element;
	element.col = element.row = 0;
	element.control = page;
	childControls.push_back(element);
	return true;
}

bool TDataLayout::SupportsChildTemplateInjection()
{
	return true;
}

bool TDataLayout::AddPage(TrecPointer<TPage> page, UINT row, UINT col, bool doOverride)
{
	if(!page.Get())
		return false;
	if (childControls.Size())
		childControls[0].control = page;
	else
	{
		ChildControl control;
		control.col = control.row = 0;
		control.control = page;
		childControls.push_back(control);
	}
}

bool TDataLayout::GetIndex(const TPoint& point, int& row, int& col)
{
	if (!DrawingBoard::IsContained(point, area))
		return false;

	if (childControls.Size() == 0 || !childControls[0].control.Get())
		return false;

	auto control = childControls[0].control;
	RECT_F chArea = control->GetArea();
	TPoint nPoint(point.x - area.left, point.y - area.top);

	UINT tempWidth = width;
	if (!width)
		tempWidth = primaryStack ? (area.right - area.left) : (chArea.right - chArea.left);
	UINT tempHeight = height;
	if (!height)
		tempHeight = primaryStack ? (chArea.bottom - chArea.top) : (area.bottom - area.top);

	if (!tempHeight || !tempWidth)
		return false;

	col = static_cast<int>(nPoint.x) / tempWidth;
	row = static_cast<int>(nPoint.y) / tempHeight;

	return true;
}

RECT_F TDataLayout::GetAreaDefault()
{
	RECT_F ret = area;
	UINT actHeight = height;
	UINT actWidth = width;
	if (var.Get())
	{
		if (stackFirst)
		{
			actHeight *= (var.Get())->GetSize();
			UINT maxWidth = 1;
			TrecPointer<TVariable> value;
			for (UINT Rust = 0; dynamic_cast<TContainerVariable*>(var.Get())->GetValueAt(Rust, value); Rust++)
			{
				if (dynamic_cast<TContainerVariable*>(value.Get()))
					maxWidth = std::max(maxWidth, value->GetSize());
			}

			actWidth *= maxWidth;
		}
		else
		{
			actWidth *= (var.Get())->GetSize();
			UINT maxHeight = 1;
			TrecPointer<TVariable> value;
			for (UINT Rust = 0; dynamic_cast<TContainerVariable*>(var.Get())->GetValueAt(Rust, value); Rust++)
			{
				if (dynamic_cast<TContainerVariable*>(value.Get()))
					maxHeight = std::max(maxHeight, value->GetSize());
			}

			actHeight *= maxHeight;
		}
	}
	ret.bottom = ret.top + actHeight;
	ret.right = ret.left + actWidth;
	return ret;
}

RECT_F TDataLayout::GetAreaByContainer()
{
	RECT_F ret = area;

	UINT primLim = stackFirst ? ((ret.bottom - ret.top) / height) : ((ret.right - ret.left) / width);

	if (!primLim)
		primLim = 1;

	UINT secCount = 1;

	if (dynamic_cast<TContainerVariable*>(var.Get()))
	{
		UINT totalCount = var->GetSize();

		secCount = totalCount / primLim + (totalCount % primLim ? 1 : 0);
	}


	if (stackFirst)
	{
		ret.bottom = ret.top + (primLim * height);
		ret.right = ret.left + (secCount * width);
	}
	else
	{
		ret.bottom = ret.top + (secCount * height);
		ret.right = ret.left + (primLim * width);
	}
	return ret;
}

RECT_F TDataLayout::GetAreaByCount()
{
	return RECT_F();
}

int TDataLayout::ConvertCoordinates(int row, int col)
{
	if (org == data_org::do_default)
	{
		return stackFirst ? row : col;
	}
	else if (org == data_org::do_limit_by_space)
	{
		UINT primLim = stackFirst ? ((area.bottom - area.top) / height) : ((area.right - area.left) / width);

		if (stackFirst)
		{
			return (col * primLim) + row;
		}
		else
			return (row * primLim) + col;
	}



	return -1;
}
