#include "..\Public\Layer.h"
#include "..\Public\GameObject.h"

CLayer::CLayer()
{
}

HRESULT CLayer::Add_GameObjects(const _tchar* pGameObjectTag, CGameObject* pGameObject)
{
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	if (nullptr != Find_GameObject(pGameObjectTag))
		return E_FAIL;

	m_GameObjects.insert({ pGameObjectTag, pGameObject });

	return S_OK;
}

void CLayer::Tick(_double dTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
		pGameObject.second->Tick(dTimeDelta);
}

void CLayer::Late_Tick(_double dTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
		pGameObject.second->Late_Tick(dTimeDelta);
}

CGameObject* CLayer::Find_GameObject(const _tchar* pGameObjectTag)
{
	auto	iter = find_if(m_GameObjects.begin(), m_GameObjects.end(), CTag_Finder(pGameObjectTag));

	if (iter == m_GameObjects.end())
		return nullptr;

	return iter->second;
}

CLayer* CLayer::Create()
{
	return new CLayer();
}

void CLayer::Free()
{
	for (auto& pGameObject : m_GameObjects)
		Safe_Release(pGameObject.second);

	m_GameObjects.clear();
}

