#include "..\Public\Composite.h"
#include "Component_Manager.h"
#include "Object_Manager.h"
#include "GameObject.h"

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

GAMEEVENT CComposite::Late_Tick(_double dTimeDelta)
{
	for (auto& Pair : m_Components)
		Pair.second->Late_Tick(dTimeDelta);

	return GAME_NOEVENT;
}

HRESULT CComposite::Reset()
{
	for (auto& Pair : m_Components)
		Pair.second->Reset();

	return S_OK;
}

HRESULT CComposite::Add_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, _Inout_ CComponent** ppOut, void* pArg)
{
	CComponent_Manager* pComponentManager = CComponent_Manager::GetInstance();
	Safe_AddRef(pComponentManager);

	CComponent* pComponent = pComponentManager->Clone_Component(iLevelIndex, pPrototypeTag, pArg);

	if (nullptr == pComponent)
		return E_FAIL;

	pComponent->Set_Owner(this);

	m_Components.emplace(pComponentTag, pComponent);

	*ppOut = pComponent;

	Safe_AddRef(pComponent);

	Safe_Release(pComponentManager);

	return S_OK;
}

HRESULT CComposite::Add_Part(const _tchar* pPrototypeTag, const _tchar* pLayerTag, const _tchar* pObjectTag, _Inout_ CGameObject** ppOut, void* pArg)
{
	CObject_Manager* pObjectManager = CObject_Manager::GetInstance();
	Safe_AddRef(pObjectManager);

	CGameObject* pGameObject = pObjectManager->Clone_GameObject(pPrototypeTag, pArg);

	pGameObject->Set_Owner(this);

	pGameObject->Set_LayerTag(pLayerTag);

	pGameObject->Set_Tag(pObjectTag);
	
	m_Components.emplace(pObjectTag , pGameObject);

	*ppOut = pGameObject;

	Safe_AddRef(pGameObject);

	Safe_Release(pObjectManager);

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
	// 컴포넌트를 찾지 못했으면 삭제.
	return E_FAIL;
}

CComponent* CComposite::Find_Component(const _tchar* pComponentTag)
{
	auto iter = find_if(m_Components.begin(), m_Components.end(), CTag_Finder(pComponentTag));

	if (m_Components.end() == iter)
		return nullptr;

	return iter->second;
}

void CComposite::Free()
{
	__super::Free();
	
	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);
	m_Components.clear();
}
