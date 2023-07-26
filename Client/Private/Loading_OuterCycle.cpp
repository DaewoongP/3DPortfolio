#include "..\Public\Loading_OuterCycle.h"
#include "GameInstance.h"

CLoading_OuterCycle::CLoading_OuterCycle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)\
	: CUI(pDevice, pContext)
{
}

CLoading_OuterCycle::CLoading_OuterCycle(const CLoading_OuterCycle& rhs)
	: CUI(rhs)
{
}

HRESULT CLoading_OuterCycle::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoading_OuterCycle::Initialize(void* pArg)
{
	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fSizeX = 274.f * 0.5f;
	m_fSizeY = 274.f * 0.5f;

	m_fX = g_iWinSizeX * 0.5f;
	m_fY = g_iWinSizeY * 0.8f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CLoading_OuterCycle::Tick(_double dTimeDelta)
{
	m_pTransformCom->Turn(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(90.f), dTimeDelta);

	__super::Tick(dTimeDelta);
}

GAMEEVENT CLoading_OuterCycle::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);

	return GAME_NOEVENT;
}

HRESULT CLoading_OuterCycle::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);

	m_pVIBufferCom->Render();

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoading_OuterCycle::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading_OuterCycle"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX("Failed BackGround Add_Component : (Com_Texture)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoading_OuterCycle::SetUp_ShaderResources()
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

CLoading_OuterCycle* CLoading_OuterCycle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLoading_OuterCycle* pInstance = new CLoading_OuterCycle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CLoading_OuterCycle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLoading_OuterCycle::Clone(void* pArg)
{
	CLoading_OuterCycle* pInstance = new CLoading_OuterCycle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CLoading_OuterCycle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoading_OuterCycle::Free()
{
	__super::Free();
}