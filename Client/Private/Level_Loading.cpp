#include "..\Public\Level_Loading.h"
#include "Loader.h"
#include "Level_Logo.h"
#include "Level_Stage1.h"
#include "Level_Stage2.h"
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
		if (FAILED(Initialize_Logo()))
		{
			MSG_BOX("Failed Loading Logo Texture");
			return E_FAIL;
		}
		break;
	case LEVEL_STAGE1:
		break;
	case LEVEL_STAGE2:
		break;
	}
	
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

	if (nullptr != m_pLoadingObject)
		m_pLoadingObject->Tick(dTimeDelta);

#ifdef _DEBUG
	SetWindowText(g_hWnd, m_pLoader->Get_LoadingText());
#endif //_DEBUG
}

HRESULT CLevel_Loading::Render()
{
	if (nullptr != m_pLoadingObject)
		if (FAILED(m_pLoadingObject->Render()))
			return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CLevel_Loading::Initialize_Logo()
{
	m_pLoadingObject = CLoading_Logo::Create(m_pDevice, m_pContext);
	
	if (nullptr == m_pLoadingObject)
		return E_FAIL;

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
	Safe_Release(m_pLoadingObject);
}
