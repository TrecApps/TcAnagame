#include "TArenaView.h"



TArenaView::TArenaView(TrecPointer<DrawingBoard> rt, TDataMap<TDataMap<TString>> ta, CAMERA_TYPE lookTo)
	: TControl(rt, ta)
{
	//cameraType = lookTo;
	//windowHandle = h;
	//instanceHandle = GetModuleHandle(nullptr);
	//viewport = NULL;
	//projector = DirectX::XMMatrixPerspectiveFovLH(0.25f * DirectX::XM_PI, 800.0f / 600.0f, 1.0f, 1000.0f);

	//location_3 = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);

	//direction_3 = DirectX::XMFLOAT3(0.001f, 0.0f, 0.0f);
	//up_3 = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
	//RefreshVectors();
}

TArenaView::~TArenaView()
{

}

//TrecPointer<TArenaEngine> TArenaView::getEngine()
//{
//	return arenaEngine;
//}

bool TArenaView::onCreate(const RECT_F& loc, TrecPointer<TFileShell> d)
{
	//TObject::ThreadLock();
	//// Don't want 3D content automatically overriden by 2D Background
	//TControl::onCreate(loc, d);


	//float aspect = static_cast<float>(loc.right - loc.left) / static_cast<float>(loc.bottom - loc.top);

	//projector = DirectX::XMMatrixPerspectiveFovLH(0.25f * DirectX::XM_PI, aspect, 1.0f, 1000.0f);

	//viewport = new D3D11_VIEWPORT;
	//viewport->TopLeftX = loc.left;
	//viewport->TopLeftY = loc.top;
	//viewport->Height = loc.bottom - loc.top;
	//viewport->Width = loc.right - loc.left;
	//viewport->MinDepth = 0.0f;
	//viewport->MaxDepth = 1.0f;

	//TString valpoint;

	//if (attributes.retrieveEntry(TString(L"EngineID"), valpoint) && d3d.Get())
	//{
	//	arenaEngine = TrecPointerKey::GetNewTrecPointer<TArenaEngine>(d3d, valpoint);//   ArenaEngine::GetArenaEngine(*(valpoint.Get()), windowHandle, GetModuleHandle(nullptr));
	//	if (!arenaEngine.Get())
	//	{
	//		TObject::ThreadRelease();
	//		return false;
	//	}
	//}

	//if (attributes.retrieveEntry(TString(L"CameraType"), valpoint))
	//{
	//	if (!valpoint.Compare(L"LookAt"))
	//		lookTo = false;
	//}

	//if (attributes.retrieveEntry(TString(L"LockLookAt"), valpoint))
	//{
	//	if (!valpoint.CompareNoCase(L"true"))
	//		lockLookAt = true;
	//}

	//if (attributes.retrieveEntry(TString(L"StartingDirection"), valpoint))
	//{
	//	TrecPointer<TDataArray<TString>> numbers = valpoint.split(TString(L","));
	//	if (numbers.Get() && numbers->Size() > 2)
	//	{
	//		float numbers3[3] = { 0.0f, 0.0f, 0.0f };

	//		TString curString = numbers->at(0);
	//		if (curString.GetSize())
	//		{
	//			if (!curString.ConvertToFloat(numbers3[0]))
	//				direction_3.x = numbers3[0];
	//		}

	//		curString = numbers->at(1);
	//		if (curString.GetSize())
	//		{
	//			if (!curString.ConvertToFloat(numbers3[1]))
	//				direction_3.y = numbers3[1];
	//		}

	//		curString = numbers->at(2);
	//		if (curString.GetSize())
	//		{
	//			if (!curString.ConvertToFloat(numbers3[2]))
	//				direction_3.z = numbers3[2];
	//		}

	//		IF_IS_LOOK_TO(lookTo)
	//		{
	//			float maxVal = max(abs(direction_3.x), max(abs(direction_3.y), abs(direction_3.z)));
	//			if (maxVal > 1.0f)
	//			{
	//				direction_3.x /= maxVal;
	//				direction_3.y /= maxVal;
	//				direction_3.z /= maxVal;
	//			}
	//		}
	//	}
	//}


	//if (attributes.retrieveEntry(TString(L"StartingLocation"), valpoint))
	//{
	//	TrecPointer<TDataArray<TString>> numbers = valpoint.split(TString(L","));
	//	if (numbers.Get() && numbers->Size() > 2)
	//	{
	//		float numbers3[3] = { 0.0f,0.0f,0.0f };

	//		TString curString = numbers->at(0);
	//		if (curString.GetSize())
	//		{
	//			if (!curString.ConvertToFloat(numbers3[0]))
	//				location_3.x = numbers3[0];
	//		}

	//		curString = numbers->at(1);
	//		if (curString.GetSize())
	//		{
	//			if (!curString.ConvertToFloat(numbers3[1]))
	//				location_3.y = numbers3[1];
	//		}
	//		curString = numbers->at(2);
	//		if (curString.GetSize())
	//		{
	//			if (!curString.ConvertToFloat(numbers3[2]))
	//				location_3.z = numbers3[2];
	//		}
	//	}
	//}

	//if (attributes.retrieveEntry(TString(L"Up"), valpoint))
	//{
	//	TrecPointer<TDataArray<TString>> numbers = valpoint.split(TString(L","));
	//	if (numbers.Get() && numbers->Size() > 2)
	//	{
	//		float numbers3[3] = { up_3.x, up_3.y,up_3.z };
	//		for (UINT c = 0; c < 3; c++)
	//		{
	//			TString curString = numbers->at(c);
	//			if (curString.GetSize())
	//			{
	//				if (curString.ConvertToFloat(numbers3[c]))
	//				{
	//					if (c == 1)
	//						numbers3[c] = 1.0f;
	//					else
	//						numbers3[c] = 0.0f;
	//				}
	//			}
	//		}

	//	}
	//}
	//RefreshVectors();

	//bool ret = arenaEngine.Get() && viewport;
	//TObject::ThreadRelease();
	return false;
}

void TArenaView::Draw(TrecPointer<TVariable> object)
{
	//TObject::ThreadLock();
	////TControl::onDraw();
	//if (content.Get())
	//	content->onDraw(area);

	//if (arenaEngine.Get() && viewport)
	//{
	//	arenaEngine->RenderScene(projector, camera, *viewport);
	//}

	//if (text.Get())
	//	text->OnDraw(object);
	//if (border.Get())
	//	border->onDraw(TControl::area);

	TObject::ThreadRelease();
}

void TArenaView::OnResize(RECT_F& r, UINT nFlags, TDataArray<EventID_Cred>& eventAr)
{
	//TObject::ThreadLock();
	//TControl::OnResize(r, nFlags, eventAr);
	//if (viewport)
	//{
	//	viewport->TopLeftX = r.left;
	//	viewport->TopLeftY = r.top;
	//	viewport->Height = r.bottom - r.top;
	//	viewport->Width = r.right - r.left;
	//}
	//TObject::ThreadRelease();
}

bool TArenaView::OnScroll(bool b, const TPoint& point, const TPoint& direction, TDataArray<EventID_Cred>& args)
{
	//if (!IsContained(point, TControl::area))
	//	return false;

	//// DirectX::XMFLOAT3 movment = TCamera::di

	//if (cameraType == LOOK_AT && !lockLookAt)
	//{
	//	DirectX::XMFLOAT3 movment{
	//		TCamera::direction_3.x - TCamera::location_3.x,
	//		TCamera::direction_3.y - TCamera::location_3.y,
	//		TCamera::direction_3.z - TCamera::location_3.z };
	//	direction_3.x += (location_3.x * direction.y);
	//	direction_3.y += (location_3.y * direction.y);
	//	direction_3.z += (location_3.z * direction.y);
	//}

	//Translate(direction.y, direction_3);

	//TControl::OnScroll(b, point, direction, args);
	return true;
}
