#include "..\Public\Window_Navigation.h"

CWindow_Navigation::CWindow_Navigation()
{
}

HRESULT CWindow_Navigation::Initialize(void* pArg)
{

	return S_OK;
}

void CWindow_Navigation::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
	ImGui::Begin("Navigation", nullptr, m_WindowFlag);



	ImGui::End();
}

CWindow_Navigation* CWindow_Navigation::Create(void* pArg)
{
	CWindow_Navigation* pInstance = New CWindow_Navigation();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CWindow_Navigation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWindow_Navigation::Free()
{
	__super::Free();
}
