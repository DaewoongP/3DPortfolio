#include "MainApp.h"
#include "GameInstance.h"
#include "Level_Loading.h"

CMainApp::CMainApp()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{
#ifdef _DEBUG

	if (::AllocConsole() == TRUE)
	{
		FILE* nfp[3];
		freopen_s(nfp + 0, "CONOUT$", "rb", stdin);
		freopen_s(nfp + 1, "CONOUT$", "wb", stdout);
		freopen_s(nfp + 2, "CONOUT$", "wb", stderr);
		std::ios::sync_with_stdio();
	}

#endif // _DEBUG

	GRAPHICDESC		GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof GraphicDesc);

	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.iViewportSizeX = g_iWinSizeX;
	GraphicDesc.iViewportSizeY = g_iWinSizeY;
	GraphicDesc.eWinMode = GRAPHICDESC::WINMODE::WM_WIN;

	FAILED_CHECK_RETURN_MSG(m_pGameInstance->Initialize_Engine(LEVEL_END, GraphicDesc, &m_pDevice, &m_pContext), E_FAIL,
		L"Failed Initialize_Engine");
	
	FAILED_CHECK_RETURN(Open_Level(LEVEL_LOGO), E_FAIL);
	
	return S_OK;
}

void CMainApp::Tick(_double dTimeDelta)
{
	if (nullptr == m_pGameInstance)
		return;

	m_pGameInstance->Tick_Engine(dTimeDelta);
}

HRESULT CMainApp::Render()
{
	NULL_CHECK_RETURN(m_pGameInstance, E_FAIL);

	FAILED_CHECK_RETURN(m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Clear_DepthStencil_View(), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Present(), E_FAIL);

	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVELID eLevelIndex)
{
	NULL_CHECK_RETURN(m_pGameInstance, E_FAIL);

	return m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eLevelIndex));
}

CMainApp* CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created CMainApp");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMainApp::Free()
{
	FreeConsole();
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	Safe_Release(m_pGameInstance);
	CGameInstance::Release_Engine(); 
}
