#include "..\Public\Object_Manager.h"
#include "GameObject.h"
#include "Layer.h"

IMPLEMENT_SINGLETON(CObject_Manager)

CObject_Manager::CObject_Manager()
{

}

CGameObject* CObject_Manager::Get_LastGameObject()
{
	return m_pLastGameObject;
}

HRESULT CObject_Manager::Reserve_Containers(_uint iNumLevels)
{
	if (nullptr != m_pLayers)
	{
		MSG_BOX("Failed CObject_Manager Reserve Containers");
		return E_FAIL;
	}

	m_pLayers = new LAYERS[iNumLevels];

	m_iNumLevels = iNumLevels;

	return S_OK;
}

HRESULT CObject_Manager::Add_Prototype(const _tchar* pPrototypeTag, CGameObject* pPrototype)
{
	if (nullptr != Find_Prototype(pPrototypeTag))
	{
		MSG_BOX("Already have Prototype In CObject_Manager");
		return E_FAIL;
	}

	m_Prototypes.emplace(pPrototypeTag, pPrototype);

	return S_OK;
}

HRESULT CObject_Manager::Add_GameObject(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pLayerTag, const _tchar* pObjectTag, void* pArg)
{
	CGameObject* pPrototype = Find_Prototype(pPrototypeTag);
	NULL_CHECK_RETURN(pPrototype, E_FAIL);

	CGameObject* pGameObject = nullptr;
	pGameObject = pPrototype->Clone(pArg);

	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	CLayer* pLayer = Find_Layer(iLevelIndex, pLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();

		if (FAILED(pLayer->Add_GameObjects(pObjectTag, pGameObject)))
		{
			Safe_Release(pLayer);
			Safe_Release(pGameObject);
			return E_FAIL;
		}

		m_pLayers[iLevelIndex].emplace(pLayerTag, pLayer);
	}
	else
	{
		if (FAILED(pLayer->Add_GameObjects(pObjectTag, pGameObject)))
		{
			Safe_Release(pGameObject);
			return E_FAIL;
		}
	}
		


	if (nullptr == m_pLastGameObject)
	{
		m_pLastGameObject = pGameObject;
		Safe_AddRef(m_pLastGameObject);
	}
	else
	{
		Safe_Release(m_pLastGameObject);
		m_pLastGameObject = pGameObject;
		Safe_AddRef(m_pLastGameObject);
	}
		

	return S_OK;
}

void CObject_Manager::Clear_LevelResources(_uint iLevelIndex)
{
	for (auto& Pair : m_pLayers[iLevelIndex])
		Safe_Release(Pair.second);

	m_pLayers[iLevelIndex].clear();
}

CGameObject* CObject_Manager::Find_GameObject(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pGameObjectTag)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, pLayerTag);
	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Find_GameObject(pGameObjectTag);
}

void CObject_Manager::Tick(_double dTimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			Pair.second->Tick(dTimeDelta);
		}
	}
}

void CObject_Manager::Late_Tick(_double dTimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			Pair.second->Late_Tick(dTimeDelta);
		}
	}
}

CGameObject* CObject_Manager::Find_Prototype(const _tchar* pPrototypeTag)
{
	auto	iter = find_if(m_Prototypes.begin(), m_Prototypes.end(), CTag_Finder(pPrototypeTag));

	if (iter == m_Prototypes.end())
		return nullptr;

	return iter->second;
}

CLayer* CObject_Manager::Find_Layer(_uint iLevelIndex, const _tchar* pLayerTag)
{
	auto	iter = find_if(m_pLayers[iLevelIndex].begin(), m_pLayers[iLevelIndex].end(), CTag_Finder(pLayerTag));

	if (iter == m_pLayers[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

void CObject_Manager::Free()
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			Safe_Release(Pair.second);
		}
		m_pLayers[i].clear();
	}
	Safe_Delete_Array(m_pLayers);

	for (auto& Pair : m_Prototypes)
	{
		Safe_Release(Pair.second);
	}

	m_Prototypes.clear();

	Safe_Release(m_pLastGameObject);
}
