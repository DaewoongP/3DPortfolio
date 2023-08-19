#include "..\Public\Level_Stage2.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "MiniGame_Manager.h"
#include "Enemy.h"
#include "ColProp.h"
#include "Layer.h"
#include "Enemy_Sniper.h"

CLevel_Stage2::CLevel_Stage2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
	, m_pMiniGame_Manager{ CMiniGame_Manager::GetInstance() }
{
	Safe_AddRef(m_pMiniGame_Manager);
}

HRESULT CLevel_Stage2::Initialize()
{
	m_pMiniGame_Manager->Initialize();

	FAILED_CHECK_RETURN(__super::Initialize(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Lights(TEXT("")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Player(TEXT("Layer_Player")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Enemy(TEXT("Layer_Enemy"), TEXT("..\\..\\Resources\\GameData\\Map\\Anim\\Stage2.AnimMap")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_MiniGame(TEXT("Layer_MiniGame")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Props(TEXT("..\\..\\Resources\\GameData\\Map\\NonAnim\\Stage2.Map")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_BackGround(TEXT("Layer_BackGround")), E_FAIL);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CLayer* pLayer = pGameInstance->Find_Layer(LEVEL_STAGE2, TEXT("Layer_Enemy"));
	const unordered_map<const _tchar*, class CGameObject*> AllObjects = pLayer->Get_AllGameObject();

	for (auto& pair : AllObjects)
	{
		m_pMiniGame_Manager->Add_Enemy(dynamic_cast<CEnemy_Sniper*>(pair.second));
	}

	Safe_Release(pGameInstance);

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

HRESULT CLevel_Stage2::Ready_Lights(const _tchar* pFilePath)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// 스테이지 있는 빛이 있을 경우 삭제하고 시작. 
	if (FAILED(pGameInstance->Clear_Lights()))
		return E_FAIL;

	CLight::LIGHTDESC DirLightDesc;
	ZEROMEM(&DirLightDesc);
	DirLightDesc.eType = CLight::TYPE_DIRECTIONAL;
	DirLightDesc.vDir = _float4(1.f, -1.f, 1.f, 0.f);
	DirLightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	DirLightDesc.vSpecular = DirLightDesc.vDiffuse;
	DirLightDesc.vAmbient = DirLightDesc.vDiffuse;
	pGameInstance->Add_Lights(DirLightDesc);

	Safe_Release(pGameInstance);

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

HRESULT CLevel_Stage2::Ready_Layer_Enemy(const _tchar* pLayerTag, const _tchar* pFilePath)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	HANDLE hFile = CreateFile(pFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong	dwByte = 0;
	_ulong	dwStrByte = 0;

	_uint iSize = 0;
	ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iSize; ++i)
	{
		// Object Tag
		_tchar wszName[MAX_PATH] = TEXT("");
		ReadFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
		if (0 == dwByte)
			break;
		ReadFile(hFile, wszName, dwStrByte, &dwByte, nullptr);
		if (0 == dwByte)
		{
			MSG_BOX("Failed Read String Data");
			return E_FAIL;
		}

		// Model Prototype Tag
		CEnemy::ENEMYDESC EnemyDesc;
		ReadFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
		if (0 == dwByte)
			break;
		ReadFile(hFile, EnemyDesc.pModelPrototypeTag, dwStrByte, &dwByte, nullptr);
		if (0 == dwByte)
		{
			MSG_BOX("Failed Read String Data");
			return E_FAIL;
		}

		// Object State
		ReadFile(hFile, &(EnemyDesc.vScale), sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &(EnemyDesc.vRotation), sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &(EnemyDesc.vPosition), sizeof(_float4), &dwByte, nullptr);

		// Enemy_Hammer
		if (nullptr != wcswcs(EnemyDesc.pModelPrototypeTag, TEXT("Enemy_Sniper")))
		{
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE2,
				TEXT("Prototype_GameObject_Enemy_Sniper"), pLayerTag, wszName, &EnemyDesc)))
			{
				MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Enemy_Sniper)");
				return E_FAIL;
			}
		}
	}

#ifdef _DEBUG
	MSG_BOX("Enemy File Load Success");
#endif // _DEBUG

	CloseHandle(hFile);

	Safe_Release(pGameInstance);

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

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE2, TEXT("Prototype_GameObject_MiniGame_Cursor"), pLayerTag, TEXT("GameObject_MiniGame_Cursor"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_MiniGame_Cursor)");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE2, TEXT("Prototype_GameObject_MiniGame_Score"), pLayerTag, TEXT("GameObject_MiniGame_Score"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_MiniGame_Score)");
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Stage2::Ready_Layer_Props(const _tchar* pFilePath)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	HANDLE hFile = CreateFile(pFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong	dwByte = 0;
	_ulong	dwStrByte = 0;

	_uint iSize = 0;
	ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iSize; ++i)
	{
		// Object Tag
		_tchar wszName[MAX_PATH] = TEXT("");
		ReadFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
		if (0 == dwByte)
			break;
		ReadFile(hFile, wszName, dwStrByte, &dwByte, nullptr);
		if (0 == dwByte)
		{
			MSG_BOX("Failed Read String Data");
			return E_FAIL;
		}

		// Model Prototype Tag
		CProp::PROPDESC PropDesc;
		ReadFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
		if (0 == dwByte)
			break;
		ReadFile(hFile, PropDesc.pModelPrototypeTag, dwStrByte, &dwByte, nullptr);
		if (0 == dwByte)
		{
			MSG_BOX("Failed Read String Data");
			return E_FAIL;
		}

		// Object State
		ReadFile(hFile, &(PropDesc.vScale), sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &(PropDesc.vRotation), sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &(PropDesc.vPosition), sizeof(_float4), &dwByte, nullptr);

		_bool bUseCollider = { false };
		ReadFile(hFile, &bUseCollider, sizeof(_bool), &dwByte, nullptr);

		if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE2,
			TEXT("Prototype_GameObject_Prop"), TEXT("Layer_Props"), wszName, &PropDesc)))
		{
			MSG_BOX("Failed Add_GameObject : (GameObject_Props)");
			return E_FAIL;
		}
	}

#ifdef _DEBUG
	MSG_BOX("Prop File Load Success");
#endif // _DEBUG

	CloseHandle(hFile);

	Safe_Release(pGameInstance);

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
