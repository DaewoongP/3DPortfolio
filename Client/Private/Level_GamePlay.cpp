#include "..\Public\Level_GamePlay.h"
#include "GameInstance.h"
#include "Prop.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	FAILED_CHECK_RETURN(__super::Initialize(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Player(TEXT("Layer_Player")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Katana(TEXT("Layer_Katana")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Props(TEXT("Layer_Props")), E_FAIL);
	
	
#ifdef _DEBUG
	FAILED_CHECK_RETURN(Ready_Layer_Camera(TEXT("Layer_Camera")), E_FAIL);
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
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Player"), pLayerTag, TEXT("GameObject_Player"))))
		return E_FAIL;

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Katana(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Katana"), pLayerTag, TEXT("GameObject_Katana"))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Props(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	HANDLE hFile = CreateFile(TEXT("..\\..\\Resources\\GameData\\Map\\Stage1.Map"), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

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

		_float3 vScale;
		_float3 vRotation;
		// Object State
		ReadFile(hFile, &(vScale), sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &(vRotation), sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &(PropDesc.vPosition), sizeof(_float4), &dwByte, nullptr);

		if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY,
			TEXT("Prototype_GameObject_Prop"), TEXT("Layer_Prop"), wszName, &PropDesc)))
		{
			MSG_BOX("Failed Add GameObject Prop");
			return E_FAIL;
		}
	}
#ifdef _DEBUG
	MSG_BOX("File Load Success");
#endif // _DEBUG	

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Camera_Free"), pLayerTag, TEXT("GameObject_Camera_Free"))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Debug(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Terrain"), pLayerTag, TEXT("GameObject_Terrain"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_ForkLift"), pLayerTag, TEXT("GameObject_ForkLift"))))
		return E_FAIL;

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
