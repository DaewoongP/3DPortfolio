#include "MainApp.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"

CMainApp::CMainApp()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	ZeroMemory(m_szFPS, sizeof(TCHAR) * MAX_STR);
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

	FAILED_CHECK_RETURN_MSG(m_pGameInstance->Initialize_Engine(g_hInst, LEVEL_END, GraphicDesc, &m_pDevice, &m_pContext), E_FAIL,
		L"Failed Initialize_Engine");

	FAILED_CHECK_RETURN(Ready_Prototype_Component_For_Static(), E_FAIL);
	
	FAILED_CHECK_RETURN(Open_Level(LEVEL_LOGO), E_FAIL);
	return S_OK;
}

void CMainApp::Tick(_double dTimeDelta)
{
	if (nullptr == m_pGameInstance)
		return;

	m_pGameInstance->Tick_Engine(dTimeDelta);

#ifdef _DEBUG
	Render_FPS(dTimeDelta);
#endif // _DEBUG
}

HRESULT CMainApp::Render()
{
	NULL_CHECK_RETURN(m_pGameInstance, E_FAIL);

	FAILED_CHECK_RETURN(m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Clear_DepthStencil_View(), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderer->Draw_RenderGroup(), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Present(), E_FAIL);

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component_For_Static()
{
	NULL_CHECK_RETURN(m_pGameInstance, E_FAIL);
	
	/* Prototype_Component_Renderer */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		m_pRenderer = CRenderer::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	Safe_AddRef(m_pRenderer);

	/* Prototype_Component_Transform */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		CTransform::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_Component_Camera */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Camera"),
		CCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_Component_Shader_VtxTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTex.hlsl"),
			VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), 
			VTXPOSNORTEX_DECL::Elements, VTXPOSNORTEX_DECL::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"),
			VTXMESH_DECL::Elements, VTXMESH_DECL::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVELID eLevelIndex)
{
	NULL_CHECK_RETURN(m_pGameInstance, E_FAIL);

	return m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eLevelIndex));
}

void CMainApp::Render_FPS(_double dTimeDelta)
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
	Safe_Release(m_pRenderer);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
	Safe_Release(m_pGameInstance);
	CGameInstance::Release_Engine(); 
}
