#include "..\Public\Crosshair.h"
#include "GameInstance.h"

CCrosshair::CCrosshair(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CCrosshair::CCrosshair(const CCrosshair& rhs)
	: CUI(rhs)
{
}

HRESULT CCrosshair::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCrosshair::Initialize(void* pArg)
{
	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fSizeX = 7.f;
	m_fSizeY = 7.f;

	// 윈도우창의 중간에 표시하게 설정.
	m_fX = g_iWinSizeX * 0.5f;
	m_fY = g_iWinSizeY * 0.5f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CCrosshair::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
}

GAMEEVENT CCrosshair::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return GAME_NOEVENT;
}

HRESULT CCrosshair::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);

	m_pVIBufferCom->Render();

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCrosshair::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Crosshair"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX("Failed BackGround Add_Component : (Com_Texture)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CCrosshair::SetUp_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}

CCrosshair* CCrosshair::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCrosshair* pInstance = new CCrosshair(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CCrosshair");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCrosshair::Clone(void* pArg)
{
	CCrosshair* pInstance = new CCrosshair(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CCrosshair");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCrosshair::Free()
{
	__super::Free();
}