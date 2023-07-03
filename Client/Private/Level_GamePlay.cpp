#include "..\Public\Level_GamePlay.h"
#include "GameInstance.h"
#include "ColProp.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	FAILED_CHECK_RETURN(__super::Initialize(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Player(TEXT("Layer_Player")), E_FAIL);
	// 스태틱메쉬 레이어를 나누기 위해 일부러 안에서 레이어를 설정함.
	FAILED_CHECK_RETURN(Ready_Layer_Props(), E_FAIL);
	
	
	// 디버깅용 카메라 및 객체.. 등등
#ifdef _DEBUG
	//FAILED_CHECK_RETURN(Ready_Layer_Camera(TEXT("Layer_Camera")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Debug(TEXT("Layer_Debug")), E_FAIL);
#endif // _DEBUG

	return S_OK;
}

void CLevel_GamePlay::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
}

HRESULT CLevel_GamePlay::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Player"), pLayerTag, TEXT("GameObject_Player"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Player)");
		return E_FAIL;
	}

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Props()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	HANDLE hFile = CreateFile(TEXT("..\\..\\Resources\\GameData\\Map\\ColliderTest.Map"), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

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
			if (wcswcs(wszName, TEXT("hook")))
			{
				if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY,
					TEXT("Prototype_GameObject_ColProp"), TEXT("Layer_Hook"), wszName, &ColPropDesc)))
				{
					MSG_BOX("Failed Add_GameObject : (GameObject_ColProps)");
					return E_FAIL;
				}
			}
			else
			{
				if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY,
					TEXT("Prototype_GameObject_ColProp"), TEXT("Layer_ColProps"), wszName, &ColPropDesc)))
				{
					MSG_BOX("Failed Add_GameObject : (GameObject_ColProps)");
					return E_FAIL;
				}
			}
		}
		else // 콜라이더가 없는 오브젝트
		{
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY,
				TEXT("Prototype_GameObject_Prop"), TEXT("Layer_Props"), wszName, &PropDesc)))
			{
				MSG_BOX("Failed Add_GameObject : (GameObject_Props)");
				return E_FAIL;
			}
		}
	}

#ifdef _DEBUG
	MSG_BOX("File Load Success");
#endif // _DEBUG

	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Camera_Free"), pLayerTag, TEXT("GameObject_Camera_Free"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Camera_Free)");
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Debug(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Terrain"), pLayerTag, TEXT("GameObject_Terrain"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Terrain)");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Blue_Snow"), pLayerTag, TEXT("GameObject_Blue_Snow"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Blue_Snow)");
		return E_FAIL;
	}

	Safe_Release(pGameInstance);
	return S_OK;
}

CLevel_GamePlay* CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_GamePlay* pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created CLevel_GamePlay");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();
}
