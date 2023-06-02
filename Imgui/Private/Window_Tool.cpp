#include "..\Public\Window_Tool.h"
#include "DummyObject.h"

CWindow_Tool::CWindow_Tool()
{
	ZEROMEM(&m_TerrainSize);
	ZEROMEM(&m_TerrainOverflowSize);
	ZEROMEM(&m_AxisState);
}

HRESULT CWindow_Tool::Initialize(void* pArg)
{
	m_vWindowSize = ImVec2(500, 200);
	
	m_TerrainSize[0] = 5;
	m_TerrainSize[1] = 5;
	m_TerrainOverflowSize[0] = 50000;
	m_TerrainOverflowSize[1] = 50000;
	m_vPickPos = _float3(0.f, 0.f, 0.f);

	m_pTerrain = static_cast<CTerrain*>(m_pGameInstance->Find_GameObject(LEVEL_TOOL, TEXT("Layer_Tool"), TEXT("GameObject_Terrain")));
	if (nullptr == m_pTerrain)
		return E_FAIL;
	else
		Safe_AddRef(m_pTerrain);

	m_pCamera_Free = static_cast<CCamera_Free*>(m_pGameInstance->Find_GameObject(LEVEL_TOOL, TEXT("Layer_Tool"), TEXT("GameObject_Camera_Free")));
	if (nullptr == m_pCamera_Free)
		return E_FAIL;
	else
		Safe_AddRef(m_pCamera_Free);

	m_pAxisUI = static_cast<CAxis*>(m_pGameInstance->Find_GameObject(LEVEL_TOOL, TEXT("Layer_Tool"), TEXT("GameObject_Axis_UI")));
	if (nullptr == m_pAxisUI)
		return E_FAIL;
	else
		Safe_AddRef(m_pAxisUI);

	m_pAxisOrigin = static_cast<CAxis*>(m_pGameInstance->Find_GameObject(LEVEL_TOOL, TEXT("Layer_Tool"), TEXT("GameObject_Axis_Origin")));
	if (nullptr == m_pAxisOrigin)
		return E_FAIL;
	else
		Safe_AddRef(m_pAxisOrigin);

	m_pAxisCenter = static_cast<CAxis*>(m_pGameInstance->Find_GameObject(LEVEL_TOOL, TEXT("Layer_Tool"), TEXT("GameObject_Axis_Center")));
	if (nullptr == m_pAxisCenter)
		return E_FAIL;
	else
		Safe_AddRef(m_pAxisCenter);

	m_pDummy = static_cast<CDummyObject*>(m_pGameInstance->Find_GameObject(LEVEL_TOOL, TEXT("Layer_Tool"), TEXT("GameObject_DummyObject")));
	if (nullptr == m_pDummy)
		return E_FAIL;
	else
		Safe_AddRef(m_pDummy);
	return S_OK;
}

void CWindow_Tool::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	Begin("Tool", nullptr, m_WindowFlag);

	TerrainSizeXZ();

	TerrainHeightMap();

	WireFrame();
	
	CamSpeedAndAxisDist();
	
	AxisRendering();
	
	MakeObject(dTimeDelta);

    End();
}

void CWindow_Tool::TerrainSizeXZ()
{
	if (InputInt2("Terrain Size X, Z", m_TerrainSize.data(), ImGuiInputTextFlags_CharsNoBlank))
	{
		if (m_TerrainOverflowSize[0] < m_TerrainSize[0] ||
			0 >= m_TerrainSize[0])
		{
			m_TerrainSize[0] = 5;
			MSG_BOX("Wrong Terrain Size X");
		}

		else if (m_TerrainOverflowSize[1] < m_TerrainSize[1] ||
			0 >= m_TerrainSize[1])
		{
			m_TerrainSize[1] = 5;
			MSG_BOX("Wrong Terrain Size Z");
		}

		else
			m_pTerrain->RemakeTerrain(m_TerrainSize[0], m_TerrainSize[1]);
	}
}

void CWindow_Tool::WireFrame()
{
	if (Checkbox("Wire Frame", &m_bIsWireFrame))
	{
		m_pTerrain->Set_WireFrame(m_bIsWireFrame);
	}
}

void CWindow_Tool::TerrainHeightMap()
{
	if (Button("Open HeightMap"))
		IMFILE->OpenDialog("ChooseHeightMap", "Choose File", ".bmp", ".");

	// display
	if (IMFILE->Display("ChooseHeightMap"))
	{
		// action if OK
		if (IMFILE->IsOk())
		{
			string str = IMFILE->GetFilePathName();

			wstring wStr = StringToWString(str);
			const _tchar* pPath = wStr.c_str();

			m_pTerrain->RemakeTerrain(pPath);
		}

		// close
		IMFILE->Close();
	}
}

void CWindow_Tool::CamSpeedAndAxisDist()
{
	SetNextItemWidth(190.f);
	if (SliderFloat("FreeCam Speed", &m_fFreeCamSpeed, 5.f, 200.f))
	{
		m_pCamera_Free->Set_Speed(m_fFreeCamSpeed);
	}

	SameLine();
	SetNextItemWidth(100.f);
	if (SliderFloat("Axis Distance", &m_fAxisDistance, 1.f, 10.f))
	{
		m_pAxisCenter->Set_AxisDistance(m_fAxisDistance);
	}
}

void CWindow_Tool::AxisRendering()
{
	if (Checkbox("Origin Axis", &m_AxisState[CAxis::AXIS_ORIGIN]))
	{
		m_pAxisOrigin->Set_Rendering(m_AxisState[CAxis::AXIS_ORIGIN]);
	}
	SameLine();
	if (Checkbox("UI Axis", &m_AxisState[CAxis::AXIS_UI]))
	{
		m_pAxisUI->Set_Rendering(m_AxisState[CAxis::AXIS_UI]);
	}
	SameLine();
	if (Checkbox("Center Axis", &m_AxisState[CAxis::AXIS_CENTER]))
	{
		m_pAxisCenter->Set_Rendering(m_AxisState[CAxis::AXIS_CENTER]);
	}
}

HRESULT CWindow_Tool::MakeObject(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON))
	{
		POINT	pt{};
		GetCursorPos(&pt);
		ScreenToClient(g_hWnd, &pt);

		_vector		vMouse;
		vMouse = XMVectorSet(pt.x / (g_iWinSizeX * 0.5f) - 1.f, pt.y / -(g_iWinSizeY * 0.5f) + 1.f, 0.f, 1.f);

		_matrix		ProjMatrix_Inverse;
		ProjMatrix_Inverse = pGameInstance->Get_TransformMatrix_Inverse(CPipeLine::D3DTRANSFORMSTATE::D3DTS_PROJ);
		vMouse = XMVector3TransformCoord(vMouse, ProjMatrix_Inverse);

		_matrix		ViewMatrix_Inverse;
		ViewMatrix_Inverse = pGameInstance->Get_TransformMatrix_Inverse(CPipeLine::D3DTRANSFORMSTATE::D3DTS_VIEW);

		_vector vRayPos, vRayDir;
		vRayPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
		vRayDir = vMouse - vRayPos;
		vRayDir = XMVector3Normalize(vRayDir);
		_vector fTest1 = XMVector4Length(vRayDir);
		vRayPos = XMVector3TransformCoord(vRayPos, ViewMatrix_Inverse);
		vRayDir = XMVector3TransformNormal(vRayDir, ViewMatrix_Inverse);

		
		if (FAILED(m_pTerrain->PickingOnTerrain(vRayPos, vRayDir, &m_vPickPos)))
			return E_FAIL;

	}

	m_pDummy->Move_Position(XMLoadFloat3(&m_vPickPos), dTimeDelta);
	Safe_Release(pGameInstance);
}


CWindow_Tool* CWindow_Tool::Create(void* pArg)
{
	CWindow_Tool* pInstance = new CWindow_Tool();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CWindow_Tool");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWindow_Tool::Free()
{
	__super::Free();
	Safe_Release(m_pDummy);
	Safe_Release(m_pTerrain);
	Safe_Release(m_pCamera_Free);
	Safe_Release(m_pAxisUI);
	Safe_Release(m_pAxisCenter);
	Safe_Release(m_pAxisOrigin);
}

