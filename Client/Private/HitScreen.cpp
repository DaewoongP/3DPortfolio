#include "..\Public\HitScreen.h"
#include "GameInstance.h"

CHitScreen::CHitScreen(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CHitScreen::CHitScreen(const CHitScreen& rhs)
	: CUI(rhs)
{
}

HRESULT CHitScreen::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHitScreen::Initialize(void* pArg)
{
	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;

	m_fX = g_iWinSizeX * 0.5f;
	m_fY = g_iWinSizeY * 0.5f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetZ(vPos, 0.1f));

	return S_OK;
}

void CHitScreen::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
}

GAMEEVENT CHitScreen::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	m_dRenderTimeAcc += dTimeDelta;

	if (m_dRenderTimeAcc < m_dRenderTime)
	{
		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SCREEN, this);
	}

	return GAME_NOEVENT;
}

HRESULT CHitScreen::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(9);

	m_pVIBufferCom->Render();

	return S_OK;
}

void CHitScreen::Render_Effect(_double dRenderTime)
{
	m_dRenderTime = dRenderTime;
	m_dRenderTimeAcc = 0.0;
}

HRESULT CHitScreen::Add_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HitScreen"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CHitScreen::SetUp_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	_float4 vColor = _float4(1.f, 0.f, 0.f, 1.f);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

CHitScreen* CHitScreen::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHitScreen* pInstance = new CHitScreen(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CHitScreen");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHitScreen::Clone(void* pArg)
{
	CHitScreen* pInstance = new CHitScreen(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CHitScreen");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHitScreen::Free()
{
	__super::Free();
}
