#include "../Public/Level_Boss.h"
#include "GameInstance.h"
#include "Enemy.h"
#include "ColProp.h"

CLevel_Boss::CLevel_Boss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Boss::Initialize()
{
	FAILED_CHECK_RETURN(__super::Initialize(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Player(TEXT("Layer_Player")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Boss(TEXT("Layer_Boss")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Enemy(TEXT("Layer_Enemy")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Props(TEXT("..\\..\\Resources\\GameData\\Map\\NonAnim\\Boss.Map")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_BackGround(TEXT("Layer_BackGround")), E_FAIL);

	return S_OK;
}

void CLevel_Boss::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
}

HRESULT CLevel_Boss::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_Player(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_BOSS, TEXT("Prototype_GameObject_Player"), pLayerTag, TEXT("GameObject_Player"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Player)");
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_Boss(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_Enemy(const _tchar* pLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_Props(const _tchar* pFilePath)
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
				if (FAILED(pGameInstance->Add_GameObject(LEVEL_BOSS,
					TEXT("Prototype_GameObject_ColProp"), TEXT("Layer_Hook"), wszName, &ColPropDesc)))
				{
					MSG_BOX("Failed Add_GameObject : (GameObject_ColProps)");
					return E_FAIL;
				}
			}
			else
			{
				if (FAILED(pGameInstance->Add_GameObject(LEVEL_BOSS,
					TEXT("Prototype_GameObject_ColProp"), TEXT("Layer_ColProps"), wszName, &ColPropDesc)))
				{
					MSG_BOX("Failed Add_GameObject : (GameObject_ColProps)");
					return E_FAIL;
				}
			}
		}
		else // 콜라이더가 없는 오브젝트
		{
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_BOSS,
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

HRESULT CLevel_Boss::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
	return S_OK;
}

CLevel_Boss* CLevel_Boss::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Boss* pInstance = new CLevel_Boss(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created CLevel_Boss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Boss::Free()
{
	__super::Free();
}
