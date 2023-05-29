#include "..\Public\Level_GamePlay.h"
#include "GameInstance.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	FAILED_CHECK_RETURN(__super::Initialize(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_BackGround(TEXT("Layer_BackGround")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Camera(TEXT("Layer_Camera")), E_FAIL);


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

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(static_cast<_uint>(LEVELID::LEVEL_GAMEPLAY), TEXT("Prototype_GameObject_Terrain"), pLayerTag)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/*CCamera_Free::CAMERAFREEDESC		CameraFreeDesc;

	CameraFreeDesc.iData = { 0 };
	CameraFreeDesc.CameraDesc.vEye = _float4(0.f, 20.f, -15.f, 1.f);
	CameraFreeDesc.CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraFreeDesc.CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);
	CameraFreeDesc.CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraFreeDesc.CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraFreeDesc.CameraDesc.fNear = 0.2f;
	CameraFreeDesc.CameraDesc.fFar = 300.f;
	CameraFreeDesc.CameraDesc.TransformDesc.SpeedPerSec = 10.f;
	CameraFreeDesc.CameraDesc.TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);


	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Camera_Free"), pLayerTag, &CameraFreeDesc)))
		return E_FAIL;*/

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
