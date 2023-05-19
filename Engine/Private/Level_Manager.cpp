#include "..\Public\Level_Manager.h"
#include "Level.h"
IMPLEMENT_SINGLETON(CLevel_Manager)

CLevel_Manager::CLevel_Manager()
{
}

HRESULT CLevel_Manager::Open_Level(_uint iLevelIndex, CLevel* pNewLevel)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* ���� �������� ���Ǵ� �ڿ��� ������. */
	if (nullptr != m_pCurrentLevel)
		pGameInstance->Clear_LevelResources(m_iLevelIndex);

	Safe_Release(pGameInstance);

	/* ���緹���� �������� ��ü */
	Safe_Release(m_pCurrentLevel);

	m_pCurrentLevel = pNewLevel;

	m_iLevelIndex = iLevelIndex;

	return S_OK;
}

void CLevel_Manager::Tick(_double dTimeDelta)
{
	if (nullptr == m_pCurrentLevel)
		return;
	
	m_pCurrentLevel->Tick(dTimeDelta);
}

void CLevel_Manager::Free()
{
	Safe_Release(m_pCurrentLevel);
}

