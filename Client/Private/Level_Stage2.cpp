#include "..\Public\Level_Stage2.h"
#include "GameInstance.h"
#include "Enemy.h"
#include "ColProp.h"

CLevel_Stage2::CLevel_Stage2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Stage2::Initialize()
{
	FAILED_CHECK_RETURN(__super::Initialize(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Player(TEXT("Layer_Player")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Enemy(TEXT("Layer_Enemy")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Props(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_BackGround(TEXT("Layer_BackGround")), E_FAIL);

	return S_OK;
}

void CLevel_Stage2::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
}

HRESULT CLevel_Stage2::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Stage2::Ready_Layer_Player(const _tchar* pLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Stage2::Ready_Layer_Enemy(const _tchar* pLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Stage2::Ready_Layer_Props()
{
	return S_OK;
}

HRESULT CLevel_Stage2::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
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
}
