#include "..\Public\Layer.h"
#include "..\Public\GameObject.h"

CLayer::CLayer()
{
}

HRESULT CLayer::Add_GameObjects(const _tchar* pGameObjectTag, CGameObject* pGameObject)
{
	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	if (nullptr != Find_GameObject(pGameObjectTag))
	{
		MSG_BOX("GameObject Tag is Already Used");
		return E_FAIL;
	}
		
	pGameObject->Set_Tag(pGameObjectTag);

	m_GameObjects.insert({ pGameObject->Get_Tag(), pGameObject });

	return S_OK;
}

CGameObject* CLayer::Find_GameObject(const _tchar* pGameObjectTag)
{
	auto	iter = find_if(m_GameObjects.begin(), m_GameObjects.end(), CTag_Finder(pGameObjectTag));

	if (iter == m_GameObjects.end())
		return nullptr;

	return iter->second;
}

HRESULT CLayer::Delete_GameObject(const _tchar* pGameObjectTag)
{
	for (auto iter = m_GameObjects.begin(); iter != m_GameObjects.end();)
	{
		if (!lstrcmp(pGameObjectTag, iter->second->Get_Tag()))
		{
			Safe_Release(iter->second);
			iter = m_GameObjects.erase(iter);
		}
		else
			++iter;
	}
	return S_OK;
}

HRESULT CLayer::Clear_Layer()
{
	for (auto& pGameObject : m_GameObjects)
		Safe_Release(pGameObject.second);

	m_GameObjects.clear();

	return S_OK;
}

void CLayer::Tick(_double dTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
		pGameObject.second->Tick(dTimeDelta);
}

GAMEEVENT CLayer::Late_Tick(_double dTimeDelta)
{
	GAMEEVENT eGameEventFlag;
	for (auto& pGameObject : m_GameObjects)
	{
		eGameEventFlag = pGameObject.second->Late_Tick(dTimeDelta);
		if (GAME_STAGE_RESET == eGameEventFlag)
		{
			// 스테이지 초기화면 바로 리셋
			return GAME_STAGE_RESET;
		}
	}

	return GAME_NOEVENT;
}

HRESULT CLayer::ResetStage()
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (FAILED(pGameObject.second->Reset()))
			return E_FAIL;
	}

	return S_OK;
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

