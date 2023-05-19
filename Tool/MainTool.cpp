#include "pch.h"
#include "MainTool.h"
#include "MainFrm.h"
#include "ToolView.h"
#include "GameInstance.h"

#include "Terrain.h"
#include "DynamicCamera.h"

CMainTool::CMainTool()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
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

	m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f));
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
		CShader::Create(m_pDevice, m_pContext, TEXT("ShaderFiles/Shader_Test.hlsl"), VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(static_cast<_uint>(LEVELID::LEVEL_STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainTool::Ready_Prototype_Object()
{
	/* For.Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"), CTerrain::Create(m_pDevice, m_pContext))))
	{
		AfxMessageBox(TEXT("Failed Add Prototype CTerrain"));
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVELID::LEVEL_STATIC),
		TEXT("Prototype_GameObject_Terrain"), TEXT("GameObject_Terrain"))))
	{
		AfxMessageBox(TEXT("Failed Add GameObject CTerrain"));
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Camera */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera"), CDynamicCamera::Create(m_pDevice, m_pContext))))
	{
		AfxMessageBox(TEXT("Failed Add Prototype CCamera"));
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_GameObject(static_cast<_uint>(LEVELID::LEVEL_STATIC),
		TEXT("Prototype_GameObject_Camera"), TEXT("GameObject_Camera"))))
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
	Safe_Release(m_pRenderer);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	CGameInstance::Release_Engine();
}
