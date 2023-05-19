#include "pch.h"
#include "MainTool.h"
#include "ToolView.h"
#include "GameInstance9.h"

#include "Renderer.h"
#include "VIBuffer_RcCol.h"
#include "Terrain.h"
#include "DynamicCamera.h"
#include "MainFrm.h"

CMainTool::CMainTool()
	: m_pGameInstance{ CGameInstance9::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainTool::Initialize()
{	
	FAILED_CHECK_RETURN(m_pGameInstance->Ready_Graphic_Device(g_hViewWnd, WINMODE::MODE_WIN, g_iWinSizeX, g_iWinSizeY, &m_pDevice), E_FAIL);
	m_pMainFrm = static_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	m_pView = static_cast<CToolView*>(m_pMainFrm->m_MainSplitter.GetPane(0, 0));

	FAILED_CHECK_RETURN(Ready_Prototype_Component(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype_Object(), E_FAIL);

	return S_OK;
}

void CMainTool::Tick(_double dTimeDelta)
{
	if (nullptr == m_pGameInstance)
		return;

	m_pGameInstance->Tick_Engine(dTimeDelta);
}

void CMainTool::Render(void)
{
	if (nullptr == m_pGameInstance)
		return;

	m_pView->Invalidate(false);

	m_pGameInstance->Render_Begin(D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.f));

	m_pRenderer->Draw_RenderGroup();

	m_pGameInstance->Render_End();
}

HRESULT CMainTool::Ready_Prototype_Component()
{
	NULL_CHECK_RETURN(m_pGameInstance, E_FAIL);

	/* Prototype_Component_Renderer */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(
		TEXT("Prototype_Component_Renderer"),
		m_pRenderer = CRenderer::Create(m_pDevice))))
		return E_FAIL;

	/* Prototype_Component_VIBuffer_RcCol */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(
		TEXT("Prototype_Component_VIBuffer_RcCol"),
		CVIBuffer_RcCol::Create(m_pDevice))))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CMainTool::Ready_Prototype_Object()
{
	/* For.Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Terrain"), CTerrain::Create(m_pDevice))))
	{
		AfxMessageBox(TEXT("Failed Add Prototype CTerrain"));
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Terrain"), TEXT("GameObject_Terrain"))))
	{
		AfxMessageBox(TEXT("Failed Add GameObject CTerrain"));
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Camera */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Camera"), CDynamicCamera::Create(m_pDevice))))
	{
		AfxMessageBox(TEXT("Failed Add Prototype CCamera"));
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Camera"), TEXT("GameObject_Camera"))))
	{
		AfxMessageBox(TEXT("Failed Add GameObject CCamera"));
		return E_FAIL;
	}

	return S_OK;
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
	Safe_Release(m_pDevice);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pGameInstance);
	CGameInstance9::Release_Engine();
}
