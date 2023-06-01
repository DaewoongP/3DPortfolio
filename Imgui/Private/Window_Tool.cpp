#include "..\Public\Window_Tool.h"

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

	return S_OK;
}

void CWindow_Tool::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	Begin("Tool", nullptr, m_WindowFlag);

	TerrainSizeXZ();

	WireFrame();
	
	CamSpeedAndAxisDist();
	
	AxisRendering();
	
    End();
}

void CWindow_Tool::TerrainSizeXZ()
{
	if (InputInt2("Terrain Size X, Z", m_TerrainSize.data(), ImGuiInputTextFlags_CharsNoBlank))
	{
		if (m_TerrainOverflowSize[0] < m_TerrainSize[0])
		{
			m_TerrainSize[0] = m_TerrainOverflowSize[0];
			MSG_BOX("Terrain Size X overflow");
		}

		else if (m_TerrainOverflowSize[1] < m_TerrainSize[1])
		{
			m_TerrainSize[1] = m_TerrainOverflowSize[1];
			MSG_BOX("Terrain Size Z overflow");
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

void CWindow_Tool::CamSpeedAndAxisDist()
{
	SetNextItemWidth(190.f);
	if (SliderFloat("FreeCam Speed", &m_fFreeCamSpeed, 5.f, 100.f))
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
	Safe_Release(m_pTerrain);
	Safe_Release(m_pCamera_Free);
	Safe_Release(m_pAxisUI);
	Safe_Release(m_pAxisCenter);
	Safe_Release(m_pAxisOrigin);
}

