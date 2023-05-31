#include "..\Public\Window_Terrain.h"

CWindow_Terrain::CWindow_Terrain()
{
	ZEROMEM(&m_TerrainSize);
}

HRESULT CWindow_Terrain::Initialize(void* pArg)
{
	m_vWindowSize = ImVec2(300, 300);
	
	m_TerrainSize[0] = 5;
	m_TerrainSize[1] = 5;
	return S_OK;
}

void CWindow_Terrain::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	Begin("Terrain", nullptr, m_WindowFlag);

	if (InputInt2("Terrain Size X, Z", m_TerrainSize.data(), ImGuiInputTextFlags_CharsNoBlank))
	{
		//m_pGameInstance->
		//RemakeTerrain(m_TerrainSize[0], m_TerrainSize[1]);
	}
	
    End();
}

CWindow_Terrain* CWindow_Terrain::Create(void* pArg)
{
	CWindow_Terrain* pInstance = new CWindow_Terrain();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CWindow_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWindow_Terrain::Free()
{
	__super::Free();
}
