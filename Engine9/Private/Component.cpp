#include "../Public/Component.h"

CComponent::CComponent(LPDIRECT3DDEVICE9 pDevice)
	: m_pDevice(pDevice)
{
	Safe_AddRef(m_pDevice);
}

CComponent::CComponent(const CComponent& rhs)
	: m_pDevice(rhs.m_pDevice)
{
	Safe_AddRef(m_pDevice);
}

HRESULT CComponent::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CComponent::Initialize(void* pArg)
{
	return S_OK;
}

void CComponent::Free()
{
	Safe_Release(m_pDevice);
}
