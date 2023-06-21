#include "..\Public\Level_Logo.h"
#include "Level_Loading.h"
#include "GameInstance.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Logo::Initialize()
{
	FAILED_CHECK_RETURN(__super::Initialize(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_BackGround(TEXT("Layer_BackGround")), E_FAIL);

	return S_OK;
}

void CLevel_Logo::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	if (GetKeyState(VK_SPACE) & 0x8000)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVELID::LEVEL_GAMEPLAY))))
		{
			Safe_Release(pGameInstance);
			return;
		}

		Safe_Release(pGameInstance);
	}

#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("로고레벨입니다."));
#endif //_DEBUG
}

HRESULT CLevel_Logo::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	FAILED_CHECK_RETURN(pGameInstance->Add_GameObject(LEVEL_LOGO,
		TEXT("Prototype_GameObject_BackGround"), pLayerTag, TEXT("GameObject_BackGround")), E_FAIL);

	Safe_Release(pGameInstance);
	return S_OK;
}

CLevel_Logo* CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Logo* pInstance = new CLevel_Logo(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created CLevel_Logo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();
}
