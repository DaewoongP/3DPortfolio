#include "..\Public\Window_Camera.h"

CWindow_Camera::CWindow_Camera()
{
}

HRESULT CWindow_Camera::Initialize(void* pArg)
{
	m_vWindowPos = ImVec2(0, 500); // offset
	m_vWindowSize = ImVec2(100, 100);


	return S_OK;
}

void CWindow_Camera::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
	ImGui::Begin("Camera", nullptr, m_WindowFlag);



	ImGui::End();
}

CWindow_Camera* CWindow_Camera::Create(void* pArg)
{
	CWindow_Camera* pInstance = New CWindow_Camera();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CWindow_Camera");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWindow_Camera::Free()
{
	__super::Free();
}
