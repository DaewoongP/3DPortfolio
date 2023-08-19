#include "..\Public\MiniGame_ScoreBoard.h"
#include "GameInstance.h"

CMiniGame_ScoreBoard::CMiniGame_ScoreBoard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}


CMiniGame_ScoreBoard::CMiniGame_ScoreBoard(const CMiniGame_ScoreBoard& rhs)
	: CUI(rhs)
	, m_pMiniGame_Manager(CMiniGame_Manager::GetInstance())
{
	Safe_AddRef(m_pMiniGame_Manager);
}

HRESULT CMiniGame_ScoreBoard::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}


HRESULT CMiniGame_ScoreBoard::Initialize(void* pArg)
{
	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fSizeX = 512.f;
	m_fSizeY = 512.f;

	m_fX = g_iWinSizeX * 0.5f;
	m_fY = g_iWinSizeY * 0.5f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CMiniGame_ScoreBoard::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
}

GAMEEVENT CMiniGame_ScoreBoard::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (nullptr != m_pRendererCom &&
		m_pMiniGame_Manager->IsFinished())
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return GAME_NOEVENT;
}

HRESULT CMiniGame_ScoreBoard::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);

	m_pVIBufferCom->Render();

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_uint3 vScore = m_pMiniGame_Manager->Get_ScoreBoardVector();

	_itow_s(vScore.x, m_szPerfect, MAX_STR, 10);
	_itow_s(vScore.y, m_szGreat, MAX_STR, 10);
	_itow_s(vScore.z, m_szFailed, MAX_STR, 10);

	// perfect
	if (FAILED(pGameInstance->Render_Font(TEXT("Font_135"), 
		m_szPerfect,
		_float2(g_iWinSizeX * 0.5f, g_iWinSizeY * 0.25f))))
		return E_FAIL;
	// great
	if (FAILED(pGameInstance->Render_Font(TEXT("Font_135"), 
		m_szGreat,
		_float2(g_iWinSizeX * 0.5f, g_iWinSizeY * 0.4f))))
		return E_FAIL;
	// failed
	if (FAILED(pGameInstance->Render_Font(TEXT("Font_135"), 
		m_szFailed,
		_float2(g_iWinSizeX * 0.5f, g_iWinSizeY * 0.55f))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CMiniGame_ScoreBoard::Add_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		MSG_BOX("Failed BackGround Add_Component : (Com_Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		MSG_BOX("Failed BackGround Add_Component : (Com_Shader)");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
	{
		MSG_BOX("Failed BackGround Add_Component : (Com_VIBuffer)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ScoreBoard"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX("Failed BackGround Add_Component : (Com_Texture)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMiniGame_ScoreBoard::SetUp_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	return S_OK;
}

CMiniGame_ScoreBoard* CMiniGame_ScoreBoard::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMiniGame_ScoreBoard* pInstance = new CMiniGame_ScoreBoard(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CMiniGame_ScoreBoard");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMiniGame_ScoreBoard::Clone(void* pArg)
{
	CMiniGame_ScoreBoard* pInstance = new CMiniGame_ScoreBoard(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CMiniGame_ScoreBoard");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMiniGame_ScoreBoard::Free()
{
	__super::Free();

	Safe_Release(m_pMiniGame_Manager);
}