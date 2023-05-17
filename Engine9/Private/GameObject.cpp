#include "..\Public\GameObject.h"
#include "GameInstance9.h"

CGameObject::CGameObject(LPDIRECT3DDEVICE9 pDevice)
	: m_pDevice(pDevice)
{
	Safe_AddRef(m_pDevice);
}

CGameObject::CGameObject(const CGameObject& rhs)
	: m_pDevice(rhs.m_pDevice)
{
	Safe_AddRef(m_pDevice);
}

HRESULT CGameObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Initialize(void* pArg)
{
	return S_OK;
}

void CGameObject::Tick(_double TimeDelta)
{
}

void CGameObject::Late_Tick(_double TimeDelta)
{
}

HRESULT CGameObject::Render()
{
	return S_OK;
}

HRESULT CGameObject::Add_Component(const _tchar* pPrototypeTag, const _tchar* pComponentTag, _Inout_ CComponent** ppOut, void* pArg)
{
	/*CGameInstance9* pGameInstance = CGameInstance9::GetInstance();
	Safe_AddRef(pGameInstance);

	CComponent* pComponent = pGameInstance->Clone_Component(pPrototypeTag);
	if (nullptr == pComponent)
		return E_FAIL;

	m_Components.emplace(pComponentTag, pComponent);

	*ppOut = pComponent;

	Safe_AddRef(pComponent);

	Safe_Release(pGameInstance);*/

	return S_OK;
}

void CGameObject::Free()
{
	/*for (auto& Pair : m_Components)
		Safe_Release(Pair.second);
	m_Components.clear();*/

	Safe_Release(m_pDevice);
}
