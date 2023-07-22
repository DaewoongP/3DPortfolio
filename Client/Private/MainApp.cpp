#include "MainApp.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Rect_Dynamic.h"

CMainApp::CMainApp()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);

#ifdef _DEBUG
	ZeroMemory(m_szFPS, sizeof(TCHAR) * MAX_STR);
#endif // _DEBUG
}

HRESULT CMainApp::Initialize()
{
	srand((_uint)time(NULL));

	// 그래픽 디바이스를 만들기 위한 구조체 할당
	GRAPHICDESC		GraphicDesc;
	ZEROMEM(&GraphicDesc);

	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.iViewportSizeX = g_iWinSizeX;
	GraphicDesc.iViewportSizeY = g_iWinSizeY;
	GraphicDesc.eWinMode = GRAPHICDESC::WINMODE::WM_WIN;

	if (FAILED(m_pGameInstance->Initialize_Engine(g_hInst, LEVEL_END, GraphicDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component_For_Static()))
		return E_FAIL;

	if (FAILED(Ready_Fonts()))
		return E_FAIL;
	
	if (FAILED(Open_Level(LEVEL_LOGO)))
	{
		MSG_BOX("Failed Open LEVEL_LOGO");
		return E_FAIL;
	}

	return S_OK;
}

void CMainApp::Tick(_double dTimeDelta)
{
	if (nullptr == m_pGameInstance)
		return;

	// 엔진의 Tick 호출
	m_pGameInstance->Tick_Engine(dTimeDelta);

#ifdef _DEBUG
	Tick_FPS(dTimeDelta);
#endif // _DEBUG
}

HRESULT CMainApp::Render()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Clear_DepthStencil_View()))
		return E_FAIL;
	if (FAILED(m_pRenderer->Draw_RenderGroup()))
		return E_FAIL;
#ifdef _DEBUG
	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_135"), m_szFPS, _float2(1100.f, 10.f))))
		return E_FAIL;
#endif // _DEBUG
	if (FAILED(m_pGameInstance->Present()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component_For_Static()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;
	
	/* Prototype_Component_Renderer */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		m_pRenderer = CRenderer::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype  : (Prototype_Component_Renderer)");
		return E_FAIL;
	}
	Safe_AddRef(m_pRenderer);

	/* Prototype_Component_Transform */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		CTransform::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype  : (Prototype_Component_Transform)");
		return E_FAIL;
	}

	/* Prototype_Component_Camera */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Camera"),
		CCamera::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype  : (Prototype_Component_Camera)");
		return E_FAIL;
	}

	/* Prototype_Component_Shader_VtxTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTex.hlsl"),
			VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements))))
	{
		MSG_BOX("Failed Add_Prototype  : (Prototype_Component_Shader_VtxTex)");
		return E_FAIL;
	}

	/* Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype  : (Prototype_Component_VIBuffer_Rect)");
		return E_FAIL;
	}
	/* Prototype_Component_VIBuffer_Rect_Dynamic */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect_Dynamic"),
		CVIBuffer_Rect_Dynamic::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype  : (Prototype_Component_VIBuffer_Rect_Dynamic)");
		return E_FAIL;
	}

	/* Prototype_Component_Texture_Loading_Logo */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading_Logo"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Game_UI/MainMenu/LevelSelect/LevelThumbnails_0%d.png"), 2))))
	{
		MSG_BOX("Failed Add_Prototype  : (Prototype_Component_Texture_Loading_Logo)");
		return E_FAIL;
	}

	_matrix PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixTranslation(0.f, -4.f, -0.5f);
	/* For.Prototype_Component_Model_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Player"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../../Resources/ParsingData/Anim/Player.dat"), PivotMatrix))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Model_Player)");
		return E_FAIL;
	}

	PivotMatrix = XMMatrixRotationX(XMConvertToRadians(-90.f)) * XMMatrixRotationY(XMConvertToRadians(180.f));
	/* For.Prototype_Component_Model_Katana */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Katana"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../../Resources/ParsingData/Anim/Katana.dat"), PivotMatrix))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Model_Katana)");
		return E_FAIL;
	}

	PivotMatrix = XMMatrixRotationZ(XMConvertToRadians(-90.f));
	/* For.Prototype_Component_Model_Shuriken */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Shuriken"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../../Resources/ParsingData/Anim/Shuriken.dat"), PivotMatrix))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Model_Shuriken)");
		return E_FAIL;
	}

	/* For.Prototype_Component_Collider_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Collider_Sphere)");
		return E_FAIL;
	}

	/* For.Prototype_Component_Collider_AABB */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Collider_AABB)");
		return E_FAIL;
	}

	/* For.Prototype_Component_Collider_OBB */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Collider_OBB)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMainApp::Ready_Fonts()
{
	if (FAILED(m_pGameInstance->Add_Fonts(m_pDevice, m_pContext, TEXT("Font_135"), TEXT("../Bin/Resources/Fonts/135ex.spritefont"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVELID eLevelIndex)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	return m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eLevelIndex));
}

#ifdef _DEBUG
void CMainApp::Tick_FPS(_double dTimeDelta)
{
	m_dFpsTime += dTimeDelta;

	++m_iFps;

	if (1.0 <= m_dFpsTime)
	{
		swprintf_s(m_szFPS, L"FPS: %d", m_iFps);
		m_iFps = 0;
		m_dFpsTime = 0.0;
	}
}
#endif // _DEBUG

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
	Safe_Release(m_pRenderer);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
	Safe_Release(m_pGameInstance);
	CGameInstance::Release_Engine(); 
}
