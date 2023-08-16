#include "..\Public\BloodScreen.h"
#include "GameInstance.h"

CBloodScreen::CBloodScreen(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CBloodScreen::CBloodScreen(const CBloodScreen& rhs)
	: CUI(rhs)
{
}

HRESULT CBloodScreen::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBloodScreen::Initialize(void* pArg)
{
	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;

	m_fX = g_iWinSizeX * 0.5f;
	m_fY = g_iWinSizeY * 0.5f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CBloodScreen::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
}

GAMEEVENT CBloodScreen::Late_Tick(_double dTimeDelta)
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

HRESULT CBloodScreen::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(9);

	m_pVIBufferCom->Render();

	return S_OK;
}

void CBloodScreen::Render_Effect(_double dRenderTime)
{
	m_dRenderTime = dRenderTime;
	m_dRenderTimeAcc = 0.0;
	m_iTextureIndex = rand() % 6;

	_float fRandSize = 1.f - (rand() % 3) / 10.f;
	m_fSizeX = g_iWinSizeX * fRandSize;
	m_fSizeY = g_iWinSizeY * fRandSize;

	switch (rand() % 5)
	{
	case 0:
		m_fX = 100.f;
		m_fY = 100.f;
		break;
	case 1:
		m_fX = g_iWinSizeX - 100.f;
		m_fY = 100.f;
		break;
	case 2:
		m_fX = 100.f;
		m_fY = g_iWinSizeY - 100.f;
		break;
	case 3:
		m_fX = g_iWinSizeX - 100.f;
		m_fY = g_iWinSizeY - 100.f;
		break;
	case 4:
		m_fX = g_iWinSizeX * 0.5f;
		m_fY = g_iWinSizeY * 0.5f;
		break;
	default:
		m_fX = g_iWinSizeX * 0.5f;
		m_fY = g_iWinSizeY * 0.5f;
		break;
	}

	m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(
			m_fX - g_iWinSizeX * 0.5f,
			-m_fY + g_iWinSizeY * 0.5f,
			0.f,
			1.f));
}

HRESULT CBloodScreen::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Blood_Screen"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBloodScreen::SetUp_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureIndex)))
		return E_FAIL;

	_float4 vColor = _float4(1.f, 0.f, 0.f, 1.f);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

CBloodScreen* CBloodScreen::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBloodScreen* pInstance = new CBloodScreen(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBloodScreen");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBloodScreen::Clone(void* pArg)
{
	CBloodScreen* pInstance = new CBloodScreen(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CBloodScreen");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBloodScreen::Free()
{
	__super::Free();

}
