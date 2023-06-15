#include "..\Public\Window_Object.h"

CWindow_Object::CWindow_Object()
{
}

HRESULT CWindow_Object::Initialize(void* pArg)
{
	m_vWindowPos = ImVec2(-870, g_iWinSizeY);
	m_vWindowSize = ImVec2(400, 300);

	return S_OK;
}

void CWindow_Object::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
	Begin("Object", nullptr, m_WindowFlag);



	End();
}

CWindow_Object* CWindow_Object::Create(void* pArg)
{
	CWindow_Object* pInstance = New CWindow_Object();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CWindow_Object");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWindow_Object::Free()
{
	__super::Free();
}
