#include "..\Public\Level_Stage1.h"
#include "GameInstance.h"
#include "Enemy.h"
#include "ColProp.h"
#include "Level_loading.h"

CLevel_Stage1::CLevel_Stage1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Stage1::Initialize()
{
	FAILED_CHECK_RETURN(__super::Initialize(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Lights(TEXT("..\\..\\Resources\\GameData\\Light\\Stage1.Light")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Player(TEXT("Layer_Player")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Enemy(TEXT("Layer_Enemy"), TEXT("..\\..\\Resources\\GameData\\Map\\Anim\\Stage1.AnimMap")), E_FAIL);
	// 스태틱메쉬 레이어를 나누기 위해 일부러 안에서 레이어를 설정함.
	FAILED_CHECK_RETURN(Ready_Layer_Props(TEXT("..\\..\\Resources\\GameData\\Map\\NonAnim\\Stage1.Map")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_BackGround(TEXT("Layer_BackGround")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_UI(TEXT("Layer_UI")), E_FAIL);
	
	
	// 디버깅용 카메라 및 객체.. 등등
#ifdef _DEBUG
	//FAILED_CHECK_RETURN(Ready_Layer_Camera(TEXT("Layer_Camera")), E_FAIL);
	//FAILED_CHECK_RETURN(Ready_Layer_Debug(TEXT("Layer_Debug")), E_FAIL);
#endif // _DEBUG

	return S_OK;
}

void CLevel_Stage1::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKeyState(DIK_F7, CInput_Device::KEY_DOWN))
	{
		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVELID::LEVEL_BOSS))))
		{
			MSG_BOX("Failed Open LEVEL_LOADING to LEVEL_STAGE1");
			Safe_Release(pGameInstance);
			return;
		}
	}

	Safe_Release(pGameInstance);
}

HRESULT CLevel_Stage1::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Lights(const _tchar* pFilePath)
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
	DirLightDesc.vDiffuse = _float4(0.15f, 0.15f, 0.15f, 1.f);
	//DirLightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	DirLightDesc.vSpecular = DirLightDesc.vDiffuse;
	DirLightDesc.vAmbient = DirLightDesc.vDiffuse;

	if (FAILED(pGameInstance->Add_Lights(DirLightDesc)))
		return E_FAIL;

	HANDLE hFile = CreateFile(pFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong	dwByte = 0;

	_uint iSize = { 0 };
	ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

	CLight::LIGHTDESC LightDesc;

	for (_uint i = 0; i < iSize; ++i)
	{
		ZEROMEM(&LightDesc);
		ReadFile(hFile, &LightDesc, sizeof(CLight::LIGHTDESC), &dwByte, nullptr);

		if (FAILED(pGameInstance->Add_Lights(LightDesc)))
			return E_FAIL;
	}

	CloseHandle(hFile);

	MSG_BOX("Light File Load Success");

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Layer_Player(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE1, TEXT("Prototype_GameObject_Player"), pLayerTag, TEXT("GameObject_Player"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Player)");
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Layer_Enemy(const _tchar* pLayerTag, const _tchar* pFilePath)
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

		// Enemy_Pistol
		if (nullptr != wcswcs(EnemyDesc.pModelPrototypeTag, TEXT("Enemy_Pistol")))
		{
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE1,
				TEXT("Prototype_GameObject_Enemy_Pistol"), pLayerTag, wszName, &EnemyDesc)))
			{
				MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Enemy_Pistol)");
				return E_FAIL;
			}
		}

		// Enemy_Sword_Elite
		if (nullptr != wcswcs(EnemyDesc.pModelPrototypeTag, TEXT("Sword_Elite")))
		{
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE1,
				TEXT("Prototype_GameObject_Enemy_Sword"), pLayerTag, wszName, &EnemyDesc)))
			{
				MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Enemy_Sword)");
				return E_FAIL;
			}
		}

		// Enemy_Hammer
		if (nullptr != wcswcs(EnemyDesc.pModelPrototypeTag, TEXT("Enemy_Hammer")))
		{
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE1,
				TEXT("Prototype_GameObject_Enemy_Hammer"), pLayerTag, wszName, &EnemyDesc)))
			{
				MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Enemy_Hammer)");
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

HRESULT CLevel_Stage1::Ready_Layer_Props(const _tchar* pFilePath)
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

		if (true == bUseCollider)
		{
			CColProp::COLPROPDESC ColPropDesc;
			lstrcpy(ColPropDesc.pModelPrototypeTag, PropDesc.pModelPrototypeTag);
			ColPropDesc.vScale = PropDesc.vScale;
			ColPropDesc.vRotation = PropDesc.vRotation;
			ColPropDesc.vPosition = PropDesc.vPosition;			

			ReadFile(hFile, &ColPropDesc.eColliderType, sizeof(CCollider::TYPE), &dwByte, nullptr);

			void* pDesc = { nullptr };

			
			switch (ColPropDesc.eColliderType)
			{
			case Engine::CCollider::TYPE_SPHERE:
				ReadFile(hFile, &ColPropDesc.SphereDesc, sizeof(CBounding_Sphere::BOUNDINGSPHEREDESC), &dwByte, nullptr);
				break;
			case Engine::CCollider::TYPE_AABB:
				ReadFile(hFile, &ColPropDesc.AABBDesc, sizeof(CBounding_AABB::BOUNDINGAABBDESC), &dwByte, nullptr);
				break;
			case Engine::CCollider::TYPE_OBB:
				ReadFile(hFile, &ColPropDesc.OBBDesc, sizeof(CBounding_OBB::BOUNDINGOBBDESC), &dwByte, nullptr);
				break;
			}

			// 훅 레이어 따로처리.
			if (wcswcs(wszName, TEXT("hook")) ||
				wcswcs(wszName, TEXT("Hook")))
			{
				if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE1,
					TEXT("Prototype_GameObject_ColProp"), TEXT("Layer_Hook"), wszName, &ColPropDesc)))
				{
					MSG_BOX("Failed Add_GameObject : (GameObject_ColProps)");
					return E_FAIL;
				}
			}
			else
			{
				if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE1,
					TEXT("Prototype_GameObject_ColProp"), TEXT("Layer_ColProps"), wszName, &ColPropDesc)))
				{
					MSG_BOX("Failed Add_GameObject : (GameObject_ColProps)");
					return E_FAIL;
				}
			}
		}
		else // 콜라이더가 없는 오브젝트
		{
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE1,
				TEXT("Prototype_GameObject_Prop"), TEXT("Layer_Props"), wszName, &PropDesc)))
			{
				MSG_BOX("Failed Add_GameObject : (GameObject_Props)");
				return E_FAIL;
			}
		}
	}

#ifdef _DEBUG
	MSG_BOX("Prop File Load Success");
#endif // _DEBUG

	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE1, TEXT("Prototype_GameObject_Sky"), pLayerTag, TEXT("GameObject_Sky"))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Sky)");
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Stage1::Ready_UI(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE1, TEXT("Prototype_GameObject_Crosshair"), pLayerTag, TEXT("GameObject_Crosshair"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Crosshair)");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE1, TEXT("Prototype_GameObject_UI_Dash"), pLayerTag, TEXT("GameObject_UI_Dash"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_UI_Dash)");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE1, TEXT("Prototype_GameObject_UI_Minimap"), pLayerTag, TEXT("GameObject_MiniMap"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_MiniMap)");
		return E_FAIL;
	}
	
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE1, TEXT("Prototype_GameObject_UI_MiniEnemy"), pLayerTag, TEXT("GameObject_MiniEnemy"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_MiniEnemy)");
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Layer_Camera(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE1, TEXT("Prototype_GameObject_Camera_Free"), pLayerTag, TEXT("GameObject_Camera_Free"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Camera_Free)");
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Layer_Debug(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE1, TEXT("Prototype_GameObject_Terrain"), pLayerTag, TEXT("GameObject_Terrain"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Terrain)");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STAGE1, TEXT("Prototype_GameObject_Blue_Snow"), pLayerTag, TEXT("GameObject_Blue_Snow"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Blue_Snow)");
		return E_FAIL;
	}

	Safe_Release(pGameInstance);
	return S_OK;
}

CLevel_Stage1* CLevel_Stage1::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Stage1* pInstance = new CLevel_Stage1(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created CLevel_Stage1");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Stage1::Free()
{
	__super::Free();
}
