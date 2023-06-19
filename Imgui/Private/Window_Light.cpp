#include "..\Public\Window_Light.h"

CWindow_Light::CWindow_Light()
{
}

HRESULT CWindow_Light::Initialize(void* pArg)
{

	return S_OK;
}

void CWindow_Light::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
	Begin("Light", nullptr, m_WindowFlag);


	End();
}

CWindow_Light* CWindow_Light::Create(void* pArg)
{
	CWindow_Light* pInstance = New CWindow_Light();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CWindow_Light");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWindow_Light::Free()
{
	__super::Free();
}
