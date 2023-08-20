#include "..\Public\Level_Loading.h"
#include "Loader.h"
#include "Level_Logo.h"
#include "Level_Stage1.h"
#include "Level_Stage2.h"
#include "Level_Boss.h"
#include "Loading_Logo.h"
#include "GameInstance.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Loading::Initialize(LEVELID eNextLevelID)
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	m_eNextLevelID = eNextLevelID;

	switch (m_eNextLevelID)
	{
	case LEVEL_LOGO:
		if (FAILED(Loading_Logo(TEXT("Layer_Loading"))))
		{
			MSG_BOX("Failed Loading Logo Object");
			return E_FAIL;
		}
		break;
	case LEVEL_STAGE1:
		if (FAILED(Loading_Stage1(TEXT("Layer_Loading"))))
		{
			MSG_BOX("Failed Loading Stage1 Object");
			return E_FAIL;
		}
		break;
	case LEVEL_STAGE2:
		if (FAILED(Loading_Stage1(TEXT("Layer_Loading"))))
		{
			MSG_BOX("Failed Loading Stage1 Object");
			return E_FAIL;
		}
		break;
	case LEVEL_BOSS:
		if (FAILED(Loading_Stage1(TEXT("Layer_Loading"))))
		{
			MSG_BOX("Failed Loading Stage1 Object");
			return E_FAIL;
		}
		break;
	}

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	pGameInstance->Stop_AllSound();

	Safe_Release(pGameInstance);
	
	m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevelID);

	if(nullptr == m_pLoader)
		return E_FAIL;

	return S_OK;
}

void CLevel_Loading::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	if (GetKeyState(VK_RETURN) & 0x8000)
	{
		// 로딩완료 체크
 		if (false == m_pLoader->Get_Finished())
			return;

		CLevel* pLevel = { nullptr };

		switch (m_eNextLevelID)
		{
		case LEVEL_LOGO:
			pLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_STAGE1:
			pLevel = CLevel_Stage1::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_STAGE2:
			pLevel = CLevel_Stage2::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_BOSS:
			pLevel = CLevel_Boss::Create(m_pDevice, m_pContext);
			break;
		}

		if (nullptr == pLevel)
			return;

		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		if (FAILED(pGameInstance->Open_Level(m_eNextLevelID, pLevel)))
		{
			MSG_BOX("Failed open Next Level");
			Safe_Release(pGameInstance);
			return;
		}
		
		Safe_Release(pGameInstance);
		return;
	}
}

HRESULT CLevel_Loading::Render()
{
	if (LEVEL_LOGO == m_eNextLevelID)
		return S_OK;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_uint iPercent = m_pLoader->Get_LoadingPercent();

	_tchar szPercent[MAX_STR] = TEXT("");
	_itow_s(iPercent, szPercent, MAX_STR, 10);

	_float2 vPos;
	if (10 > iPercent)
		vPos = _float2(g_iWinSizeX * 0.485f, g_iWinSizeY * 0.768f);
	else if (10 <= iPercent && 
		100 > iPercent)
		vPos = _float2(g_iWinSizeX * 0.475f, g_iWinSizeY * 0.768f);
	else
		vPos = _float2(g_iWinSizeX * 0.465f, g_iWinSizeY * 0.768f);

	if (FAILED(pGameInstance->Render_Font(TEXT("Font_135"), szPercent, vPos)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

#ifdef _DEBUG
	SetWindowText(g_hWnd, m_pLoader->Get_LoadingText());
#endif //_DEBUG

	return S_OK;
}

HRESULT CLevel_Loading::Loading_Logo(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOADING, TEXT("Prototype_Object_Loading_Logo"), pLayerTag, TEXT("GameObject_Logo"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Logo)");
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Loading::Loading_Stage1(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOADING, TEXT("Prototype_GameObject_GhostRunner_Logo"), pLayerTag, TEXT("GameObject_GhostRunner_Logo"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_GhostRunner_Logo)");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOADING, TEXT("Prototype_GameObject_OuterCycle"), pLayerTag, TEXT("GameObject_Loading_OuterCycle"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Loading_OuterCycle)");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOADING, TEXT("Prototype_GameObject_InnerCycle"), pLayerTag, TEXT("GameObject_Loading_InnerCycle"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Loading_InnerCycle)");
		return E_FAIL;
	}
	
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOADING, TEXT("Prototype_GameObject_Black_BackGround"), pLayerTag, TEXT("GameObject_Black_BackGround"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Black_BackGround)");
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

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
