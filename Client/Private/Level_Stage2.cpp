#include "..\Public\Level_Stage2.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "MiniGame_Manager.h"
#include "Enemy.h"
#include "ColProp.h"

CLevel_Stage2::CLevel_Stage2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
	, m_pMiniGame_Manager{ CMiniGame_Manager::GetInstance() }
{
	Safe_AddRef(m_pMiniGame_Manager);
}

HRESULT CLevel_Stage2::Initialize()
{
	FAILED_CHECK_RETURN(__super::Initialize(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Player(TEXT("Layer_Player")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Enemy(TEXT("Layer_Enemy")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_MiniGame(TEXT("Layer_MiniGame")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Props(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_BackGround(TEXT("Layer_BackGround")), E_FAIL);

	return S_OK;
}

void CLevel_Stage2::Tick(_double dTimeDelta)
{
	m_pMiniGame_Manager->Tick(dTimeDelta);

	__super::Tick(dTimeDelta);
#ifdef _DEBUG
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKeyState(DIK_F7, CInput_Device::KEY_DOWN))
	{
		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVELID::LEVEL_BOSS))))
		{
			MSG_BOX("Failed Open LEVEL_STAGE2 to LEVEL_BOSS");
			Safe_Release(pGameInstance);
			return;
		}
	}

	Safe_Release(pGameInstance);
#endif // _DEBUG
}

HRESULT CLevel_Stage2::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Stage2::Ready_Layer_Player(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE2, TEXT("Prototype_GameObject_MiniPlayer"), pLayerTag, TEXT("GameObject_MiniPlayer"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_MiniPlayer)");
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Stage2::Ready_Layer_Enemy(const _tchar* pLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Stage2::Ready_Layer_MiniGame(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE2, TEXT("Prototype_GameObject_MiniGame_Back"), pLayerTag, TEXT("GameObject_MiniGame_Back"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_MiniGame_Back)");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE2, TEXT("Prototype_GameObject_MiniGame_Great"), pLayerTag, TEXT("GameObject_MiniGame_Great"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_MiniGame_Great)");
		return E_FAIL;
	}
	
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE2, TEXT("Prototype_GameObject_MiniGame_Perfect"), pLayerTag, TEXT("GameObject_MiniGame_Perfect"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_MiniGame_Perfect)");
		return E_FAIL;
	}


	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Stage2::Ready_Layer_Props()
{
	return S_OK;
}

HRESULT CLevel_Stage2::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE2, TEXT("Prototype_GameObject_MiniBackGround"), pLayerTag, TEXT("GameObject_MiniBackGround"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_MiniBackGround)");
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

CLevel_Stage2* CLevel_Stage2::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Stage2* pInstance = new CLevel_Stage2(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created CLevel_Stage2");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Stage2::Free()
{
	__super::Free();

	m_pMiniGame_Manager->DestroyInstance();
	Safe_Release(m_pMiniGame_Manager);
}
