#include "..\Public\Window_Terrain.h"

CWindow_Terrain::CWindow_Terrain()
{
}

HRESULT CWindow_Terrain::Initialize(void* pArg)
{
	return S_OK;
}

void CWindow_Terrain::Tick(_double dTimeDelta)
{
	ImGui::Begin("Terrain");

	//ImGui::ShowDemoWindow();

	ImGui::End();
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
