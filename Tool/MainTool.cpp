#include "pch.h"
#include "MainTool.h"
#include "MainFrm.h"
#include "ToolView.h"
#include "GameInstance.h"
#include "ToolInstance.h"

CMainTool::CMainTool()
	: m_pGameInstance{ CGameInstance::GetInstance() }
	, m_pToolInstance{ CToolInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pToolInstance);

	ZeroMemory(m_szFPS, sizeof(TCHAR) * MAX_STR);
}

HRESULT CMainTool::Initialize()
{	
	GRAPHICDESC		GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof GraphicDesc);

	GraphicDesc.hWnd = g_hViewWnd;
	GraphicDesc.iViewportSizeX = g_iWinSizeX;
	GraphicDesc.iViewportSizeY = g_iWinSizeY;
	GraphicDesc.eWinMode = GRAPHICDESC::WINMODE::WM_WIN;

	FAILED_CHECK_RETURN_MSG(m_pGameInstance->Initialize_Engine(static_cast<_uint>(LEVELID::LEVEL_END), GraphicDesc, &m_pDevice, &m_pContext), E_FAIL,
		L"Failed Initialize_Engine");
	m_pMainFrm = static_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	m_pView = static_cast<CToolView*>(m_pMainFrm->m_MainSplitter.GetPane(0, 0));
	m_pToolInstance->m_pMainFrm = m_pMainFrm;
	m_pToolInstance->m_pView = m_pView;
	
	FAILED_CHECK_RETURN(Ready_Prototype_Component(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype_Object(), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Ready_DInput(AfxGetInstanceHandle(), g_hViewWnd), E_FAIL);
	return S_OK;
}

void CMainTool::Tick(_double dTimeDelta)
{
	if (nullptr == m_pGameInstance)
		return;
	m_pGameInstance->Update_DInput();
	m_pGameInstance->Tick_Engine(dTimeDelta);
	
	Render_FPS(dTimeDelta);
}

void CMainTool::Render(void)
{
	if (nullptr == m_pGameInstance)
		return;

	m_pView->Invalidate(false);

	m_pGameInstance->Clear_BackBuffer_View(_float4(0.5f, 0.5f, 0.5f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();
	m_pRenderer->Draw_RenderGroup();
	m_pGameInstance->Present();
}

HRESULT CMainTool::Ready_Prototype_Component()
{
	NULL_CHECK_RETURN(m_pGameInstance, E_FAIL);

	/* Prototype_Component_Renderer */
	if (FAILED(m_pGameInstance->Add_Prototype(
		static_cast<_uint>(LEVELID::LEVEL_STATIC),
		TEXT("Prototype_Component_Renderer"),
		m_pRenderer = CRenderer::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	Safe_AddRef(m_pRenderer);

	/* Prototype_Component_Shader_Vtxtex */
	if (FAILED(m_pGameInstance->Add_Prototype(static_cast<_uint>(LEVELID::LEVEL_STATIC), TEXT("Prototype_Component_Shader_Vtxtex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("ShaderFiles/Shader_Texture.hlsl"), VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_Nontex */
	if (FAILED(m_pGameInstance->Add_Prototype(static_cast<_uint>(LEVELID::LEVEL_STATIC), TEXT("Prototype_Component_Shader_Nontex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("ShaderFiles/Shader_NonTex.hlsl"), VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(static_cast<_uint>(LEVELID::LEVEL_STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(static_cast<_uint>(LEVELID::LEVEL_STATIC), TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, 5, 5))))
		return E_FAIL;

	/* Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(static_cast<_uint>(LEVELID::LEVEL_STATIC), TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("Resources/ANA.png")))))
		return E_FAIL;

	/* Prototype_Component_VIBuffer_Line */
	if (FAILED(m_pGameInstance->Add_Prototype(static_cast<_uint>(LEVELID::LEVEL_STATIC), TEXT("Prototype_Component_VIBuffer_Line"),
		CVIBuffer_Line::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainTool::Ready_Prototype_Object()
{
	/* For.Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"), 
		m_pToolInstance->m_pTerrain = CTerrain::Create(m_pDevice, m_pContext))))
	{
		AfxMessageBox(TEXT("Failed Add Prototype CTerrain"));
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVELID::LEVEL_STATIC),
		TEXT("Prototype_GameObject_Terrain"), TEXT("GameObject_Terrain"), nullptr, false)))
	{
		AfxMessageBox(TEXT("Failed Add GameObject CTerrain"));
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Camera */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera"), 
		m_pToolInstance->m_pDynamicCamera = CDynamicCamera::Create(m_pDevice, m_pContext))))
	{
		AfxMessageBox(TEXT("Failed Add Prototype CCamera"));
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVELID::LEVEL_STATIC),
		TEXT("Prototype_GameObject_Camera"), TEXT("GameObject_Camera"), nullptr, false)))
	{
		AfxMessageBox(TEXT("Failed Add GameObject CCamera"));
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Axis */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Axis"),
		m_pToolInstance->m_pAxis = CAxis::Create(m_pDevice, m_pContext))))
	{
		AfxMessageBox(TEXT("Failed Add Prototype CAxis"));
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVELID::LEVEL_STATIC),
		TEXT("Prototype_GameObject_Axis"), TEXT("GameObject_Axis"), nullptr, false)))
	{
		AfxMessageBox(TEXT("Failed Add GameObject CAxis"));
		return E_FAIL;
	}

	return S_OK;
}

void CMainTool::Render_FPS(_double dTimeDelta)
{
	m_dFpsTime += dTimeDelta;
	++m_iFps;
	if (1.0 <= m_dFpsTime)
	{
		swprintf_s(m_szFPS, L"FPS: %d", m_iFps);
		SetWindowText(m_pMainFrm->m_hWnd, m_szFPS);
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
	Safe_Release(m_pRenderer);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pToolInstance);
	CToolInstance::DestroyInstance();
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	CGameInstance::Release_Engine();
}
