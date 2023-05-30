#include "MainTool.h"
#include "Level_Tool.h"

CMainTool::CMainTool()
	: m_pGameInstance(CGameInstance::GetInstance())
	, m_pImWindow_Manager(CImWindow_Manager::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pImWindow_Manager);
	ZeroMemory(m_szFPS, sizeof(TCHAR)* MAX_STR);
}

HRESULT CMainTool::Initialize()
{
	GRAPHICDESC		GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof GraphicDesc);

	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.iViewportSizeX = g_iWinSizeX;
	GraphicDesc.iViewportSizeY = g_iWinSizeY;
	GraphicDesc.eWinMode = GRAPHICDESC::WINMODE::WM_WIN;

	FAILED_CHECK_RETURN_MSG(m_pGameInstance->Initialize_Engine(g_hInst, static_cast<_uint>(LEVELID::LEVEL_END), GraphicDesc, &m_pDevice, &m_pContext), E_FAIL,
		L"Failed Initialize_Engine");

	FAILED_CHECK_RETURN(Ready_Prototype_Component(), E_FAIL);

	FAILED_CHECK_RETURN(Open_Level(), E_FAIL);

	m_pImWindow_Manager->Initialize(m_pDevice, m_pContext, &m_pIO);

	FAILED_CHECK_RETURN(Add_Windows(), E_FAIL);
	return S_OK;
}

void CMainTool::Tick(_double dTimeDelta)
{
	if (nullptr == m_pGameInstance)
		return;

	m_pGameInstance->Tick_Engine(dTimeDelta);

	m_pImWindow_Manager->Tick(dTimeDelta);
	Render_FPS(dTimeDelta);
}

HRESULT CMainTool::Render(void)
{
	NULL_CHECK_RETURN(m_pGameInstance, E_FAIL);

	FAILED_CHECK_RETURN(m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Clear_DepthStencil_View(), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderer->Draw_RenderGroup(), E_FAIL);
	m_pImWindow_Manager->Render();
	FAILED_CHECK_RETURN(m_pGameInstance->Present(), E_FAIL);

	return S_OK;
}

HRESULT CMainTool::Ready_Prototype_Component()
{
	NULL_CHECK_RETURN(m_pGameInstance, E_FAIL);

	/* Prototype_Component_Renderer */
	if (FAILED(m_pGameInstance->Add_Prototype(static_cast<_uint>(LEVELID::LEVEL_TOOL), TEXT("Prototype_Component_Renderer"),
		m_pRenderer = CRenderer::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	Safe_AddRef(m_pRenderer);

	return S_OK;
}

HRESULT CMainTool::Ready_Prototype_Object()
{

	return S_OK;
}

HRESULT CMainTool::Open_Level()
{
	NULL_CHECK_RETURN(m_pGameInstance, E_FAIL);

	return m_pGameInstance->Open_Level(static_cast<_uint>(LEVELID::LEVEL_TOOL), CLevel_Tool::Create(m_pDevice, m_pContext));
}

HRESULT CMainTool::Add_Windows()
{
	FAILED_CHECK_RETURN_MSG(m_pImWindow_Manager->Add_Window(TEXT("Window_Terrain"),
		CWindow_Terrain::Create()), E_FAIL, TEXT("Failed Create CWindow_Terrain"));

	return S_OK;
}

void CMainTool::Render_FPS(_double dTimeDelta)
{
	m_dFpsTime += dTimeDelta;
	++m_iFps;
	if (1.0 <= m_dFpsTime)
	{
		swprintf_s(m_szFPS, L"FPS: %d", m_iFps);
		SetWindowText(g_hWnd, m_szFPS);
		m_iFps = 0;
		m_dFpsTime = 0.0;
	}
}

CMainTool* CMainTool::Create()
{
	CMainTool* pInstance = new CMainTool();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created CMainTool");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMainTool::Free()
{
	Safe_Release(m_pImWindow_Manager);
	m_pImWindow_Manager->DestroyInstance();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	CGameInstance::Release_Engine();
}
