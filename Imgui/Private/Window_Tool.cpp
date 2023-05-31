#include "..\Public\Window_Tool.h"

CWindow_Tool::CWindow_Tool()
{
	ZEROMEM(&m_TerrainSize);
	ZEROMEM(&m_TerrainOverflowSize);
}

HRESULT CWindow_Tool::Initialize(void* pArg)
{
	m_vWindowSize = ImVec2(500, 100);
	
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
	return S_OK;
}

void CWindow_Tool::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	Begin("Tool", nullptr, m_WindowFlag);

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
	
	if (Checkbox("Wire Frame", &m_bIsWireFrame))
	{
		m_pTerrain->Set_WireFrame(m_bIsWireFrame);
	}

	if (SliderFloat("FreeCam Speed", &m_fFreeCamSpeed, 5.f, 100.f))
	{
		m_pCamera_Free->Set_Speed(m_fFreeCamSpeed);
	}
    End();
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
}
