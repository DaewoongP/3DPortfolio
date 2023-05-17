#include "..\Public\Object_Manager.h"
#include "../Public/GameObject.h"
#include "../Public/Layer.h"

IMPLEMENT_SINGLETON(CObject_Manager)

CObject_Manager::CObject_Manager()
{

}

HRESULT CObject_Manager::Add_Prototype(const _tchar* pPrototypeTag, CGameObject* pPrototype)
{
	if (nullptr != Find_Prototype(pPrototypeTag))
	{
		MSG_BOX("Already have Protorype In CObject_Manager");
		return E_FAIL;
	}

	m_Prototypes.emplace(pPrototypeTag, pPrototype);

	return S_OK;
}

HRESULT CObject_Manager::Add_GameObject(const _tchar* pPrototypeTag, const _tchar* pLayerTag, void* pArg)
{
	CGameObject* pPrototype = Find_Prototype(pPrototypeTag);
	NULL_CHECK_RETURN(pPrototype, E_FAIL);

	CGameObject* pGameObject = pPrototype->Clone(pArg);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	m_Layers.emplace(pLayerTag, pGameObject);

	return S_OK;
}

void CObject_Manager::Tick(_double dTimeDelta)
{
	for (auto& Pair : m_Layers)
	{
		Pair.second->Tick(dTimeDelta);
	}
}

void CObject_Manager::Late_Tick(_double dTimeDelta)
{
	for (auto& Pair : m_Layers)
	{
		Pair.second->Late_Tick(dTimeDelta);
	}
}

CGameObject* CObject_Manager::Find_Prototype(const _tchar* pPrototypeTag)
{
	auto	iter = find_if(m_Prototypes.begin(), m_Prototypes.end(), CTag_Finder(pPrototypeTag));

	if (iter == m_Prototypes.end())
		return nullptr;

	return iter->second;
}


void CObject_Manager::Free()
{
	for (auto& Pair : m_Layers)
	{
		Safe_Release(Pair.second);
	}
	m_Layers.clear();

	for (auto& Pair : m_Prototypes)
	{
		Safe_Release(Pair.second);
	}
	m_Prototypes.clear();
}
