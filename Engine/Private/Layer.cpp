#include "..\Public\Layer.h"
#include "GameObject.h"

CLayer::CLayer()
{
}

HRESULT CLayer::Add_GameObjects(CGameObject* pGameObject)
{
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	
	m_GameObjects.push_back(pGameObject);

	return S_OK;
}

void CLayer::Tick(_double TimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
		pGameObject->Tick(TimeDelta);
}

void CLayer::Late_Tick(_double TimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
		pGameObject->Late_Tick(TimeDelta);
}

CLayer* CLayer::Create()
{
	return new CLayer();
}

void CLayer::Free()
{
	for (auto& pGameObject : m_GameObjects)
		Safe_Release(pGameObject);

	m_GameObjects.clear();
}

