#include "..\Public\Window_Model.h"

CWindow_Model::CWindow_Model()
{
}

HRESULT CWindow_Model::Initialize(void* pArg)
{
	m_vWindowPos = ImVec2(-1270, g_iWinSizeY);
	m_vWindowSize = ImVec2(1260, 300);


	return S_OK;
}

void CWindow_Model::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
	Begin("Model", nullptr, m_WindowFlag);

	Select_MeshFiles();

	Setting_Transform();

	End();
}

HRESULT CWindow_Model::Select_MeshFiles()
{

	return S_OK;
}

HRESULT CWindow_Model::Setting_Transform()
{
	return S_OK;
}

CWindow_Model* CWindow_Model::Create(void* pArg)
{
	CWindow_Model* pInstance = new CWindow_Model();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CWindow_Model");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWindow_Model::Free()
{
	__super::Free();
}
