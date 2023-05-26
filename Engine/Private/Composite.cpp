#include "..\Public\Composite.h"
#include "GameInstance.h"

CComposite::CComposite(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CComposite::CComposite(const CComposite& rhs)
	: CComponent(rhs)
	, m_Components(rhs.m_Components)
{
	for (auto& Pair : m_Components)
		Safe_AddRef(Pair.second);
}

void CComposite::Tick(_double dTimeDelta)
{
	for (auto& Pair : m_Components)
		Pair.second->Tick(dTimeDelta);
}

void CComposite::Late_Tick(_double dTimeDelta)
{
	for (auto& Pair : m_Components)
		Pair.second->Late_Tick(dTimeDelta);
}

HRESULT CComposite::Render()
{
	for (auto& Pair : m_Components)
		Pair.second->Render();
	return S_OK;
}

HRESULT CComposite::Add_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, _Inout_ CComponent** ppOut, void* pArg)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CComponent* pComponent = pGameInstance->Clone_Component(iLevelIndex, pPrototypeTag, pArg);
	if (nullptr == pComponent)
		return E_FAIL;

	m_Components.emplace(pComponentTag, pComponent);

	*ppOut = pComponent;

	Safe_AddRef(pComponent);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CComposite::Delete_Component(const _tchar* pComponentTag)
{
	for (auto Pair = m_Components.begin(); Pair != m_Components.end();)
	{
		if (!lstrcmp(pComponentTag, Pair->first))
		{
			Safe_Release(Pair->second);
			Pair = m_Components.erase(Pair);

			return S_OK;
		}
		else
			++Pair;
	}

	return E_FAIL;
}

void CComposite::Free()
{
	__super::Free();
	
	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);
	m_Components.clear();

}
