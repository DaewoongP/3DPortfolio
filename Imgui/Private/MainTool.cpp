#include "MainTool.h"
#include "Level_Tool.h"
#include "Terrain.h"
#include "Camera_Free.h"
#include "Axis.h"
#include "DummyObject.h"

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
	GraphicDesc.eWinMode = GRAPHICDESC::WM_WIN;

	FAILED_CHECK_RETURN_MSG(m_pGameInstance->Initialize_Engine(g_hInst, LEVEL_END, GraphicDesc, &m_pDevice, &m_pContext), E_FAIL,
		L"Failed Initialize_Engine");

	FAILED_CHECK_RETURN(Ready_Prototype_Component(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype_Object(), E_FAIL);

	FAILED_CHECK_RETURN(Open_Level(), E_FAIL);

	m_pImWindow_Manager->Initialize(m_pDevice, m_pContext, &m_pIO);

	FAILED_CHECK_RETURN(Add_Windows(), E_FAIL);
	return S_OK;
}

void CMainTool::Tick(_double dTimeDelta)
{
	if (nullptr == m_pGameInstance)
		return;
	ShowDemoWindow();
	m_pImWindow_Manager->Tick(dTimeDelta);
	m_pGameInstance->Tick_Engine(dTimeDelta);
	
	Render_FPS(dTimeDelta);
}

HRESULT CMainTool::Render(void)
{
	NULL_CHECK_RETURN(m_pGameInstance, E_FAIL);

	FAILED_CHECK_RETURN(m_pGameInstance->Clear_BackBuffer_View(_float4(0.5f, 0.5f, 0.5f, 1.f)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Clear_DepthStencil_View(), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderer->Draw_RenderGroup(), E_FAIL);
	m_pImWindow_Manager->Render();
	m_pGameInstance->ResetRenderTargets();
	FAILED_CHECK_RETURN(m_pGameInstance->Present(), E_FAIL);

	return S_OK;
}

HRESULT CMainTool::Ready_Prototype_Component()
{
	NULL_CHECK_RETURN(m_pGameInstance, E_FAIL);

	/* Prototype_Component_Renderer */
	if (FAILED(m_pGameInstance->Add_Prototype(
		LEVEL_TOOL, TEXT("Prototype_Component_Renderer"),
		m_pRenderer = CRenderer::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	Safe_AddRef(m_pRenderer);

	/* Prototype_Component_Transform */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Transform"),
		CTransform::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_Component_Camera */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Camera"),
		CCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	
	/* Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, 5, 5))))
		return E_FAIL;

	/* Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Tile%d.dds"), 2))))
		return E_FAIL;

	/* Prototype_Component_VIBuffer_Line */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Line"),
		CVIBuffer_Line::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	

	Ready_Prototype_Component_Shader();
	Ready_Prototype_Component_Model();

	return S_OK;
}

HRESULT CMainTool::Ready_Prototype_Component_Shader()
{
	/* Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Texture.hlsl"),
			VTXPOSNORTEX_DECL::Elements, VTXPOSNORTEX_DECL::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_Nontex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_Nontex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_NonTex.hlsl"),
			VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_Cube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cube.hlsl"),
			VTXPOSCUBE_DECL::Elements, VTXPOSCUBE_DECL::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_DummyMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_DummyMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_DummyMesh.hlsl"),
			VTXMESH_DECL::Elements, VTXMESH_DECL::iNumElements))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainTool::Ready_Prototype_Component_Model()
{
	/* Prototype_Component_Model_TestWall */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_TestWall"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/TestWall.fbx"))))
		return E_FAIL;

	/* Prototype_Component_Model_Fiona */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Fiona"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Fiona.fbx"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainTool::Ready_Prototype_Object()
{
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Axis"),
		CAxis::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DummyObject"),
		CDummyObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainTool::Open_Level()
{
	NULL_CHECK_RETURN(m_pGameInstance, E_FAIL);

	return m_pGameInstance->Open_Level(LEVEL_TOOL, CLevel_Tool::Create(m_pDevice, m_pContext));
}

HRESULT CMainTool::Add_Windows()
{
	// enum == pushback
	m_pImWindow_Manager->m_ImWindows.push_back(CWindow_Tool::Create());
	m_pImWindow_Manager->m_ImWindows.push_back(CWindow_Camera::Create());
	m_pImWindow_Manager->m_ImWindows.push_back(CWindow_UI::Create());
	m_pImWindow_Manager->m_ImWindows.push_back(CWindow_Light::Create());
	m_pImWindow_Manager->m_ImWindows.push_back(CWindow_Model::Create());

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
