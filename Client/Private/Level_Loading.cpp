#include "..\Public\Level_Loading.h"
#include "Loader.h"
#include "Level_Logo.h"
#include "Level_GamePlay.h"
#include "GameInstance.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Loading::Initialize(LEVELID eNextLevelID)
{
	FAILED_CHECK_RETURN(__super::Initialize(), E_FAIL);

	m_eNextLevelID = eNextLevelID;

	m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevelID);
	NULL_CHECK_RETURN(m_pLoader, E_FAIL);
	
	return S_OK;
}

void CLevel_Loading::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	if (GetKeyState(VK_RETURN) & 0x8000)
	{
		if (false == m_pLoader->Get_Finished())
			return;

		CLevel* pLevel = { nullptr };

		switch (m_eNextLevelID)
		{
		case LEVEL_LOGO:
			pLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_GAMEPLAY:
			pLevel = CLevel_GamePlay::Create(m_pDevice, m_pContext);
			break;
		}
		NULL_CHECK_MSG(pLevel, L"NULL Level");

		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		pGameInstance->Open_Level(m_eNextLevelID, pLevel);

		Safe_Release(pGameInstance);
		return;
	}

#ifdef _DEBUG
	SetWindowText(g_hWnd, m_pLoader->Get_LoadingText());
#endif //_DEBUG
}

HRESULT CLevel_Loading::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	
	return S_OK;
}

CLevel_Loading* CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID)
{
	CLevel_Loading* pInstance = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Created CLevel_Loading");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
}
