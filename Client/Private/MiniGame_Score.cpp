#include "..\Public\MiniGame_Score.h"
#include "GameInstance.h"

CMiniGame_Score::CMiniGame_Score(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CMiniGame_Score::CMiniGame_Score(const CMiniGame_Score& rhs)
	: CUI(rhs)
	, m_pMiniGame_Manager(CMiniGame_Manager::GetInstance())
{
	Safe_AddRef(m_pMiniGame_Manager);
}

HRESULT CMiniGame_Score::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMiniGame_Score::Initialize(void* pArg)
{
	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fSizeX = 64.f;
	m_fSizeY = 128.f;

	m_fX = g_iWinSizeX * 0.5f;
	m_fY = g_iWinSizeY * 0.2f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CMiniGame_Score::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
	_uint iScore = m_pMiniGame_Manager->Get_Score();
	m_iOnes = iScore % 10;
	m_iTens = iScore / 10;
}

GAMEEVENT CMiniGame_Score::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (nullptr != m_pRendererCom &&
		false == m_pMiniGame_Manager->IsFinished())
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return GAME_NOEVENT;
}

HRESULT CMiniGame_Score::Render()
{
	Change_Position(g_iWinSizeX * 0.5f - m_fSizeX * 0.5f, m_fY);
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;
	// 10
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTens)))
		return E_FAIL;

	m_pShaderCom->Begin(1);

	m_pVIBufferCom->Render();

	// 1
	Change_Position(g_iWinSizeX * 0.5f + m_fSizeX * 0.5f, m_fY);
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;
	
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iOnes)))
		return E_FAIL;

	m_pShaderCom->Begin(1);

	m_pVIBufferCom->Render();

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMiniGame_Score::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Number"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX("Failed BackGround Add_Component : (Com_Texture)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMiniGame_Score::SetUp_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	return S_OK;
}

CMiniGame_Score* CMiniGame_Score::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMiniGame_Score* pInstance = new CMiniGame_Score(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CMiniGame_Score");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMiniGame_Score::Clone(void* pArg)
{
	CMiniGame_Score* pInstance = new CMiniGame_Score(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CMiniGame_Score");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMiniGame_Score::Free()
{
	__super::Free();

	Safe_Release(m_pMiniGame_Manager);
}