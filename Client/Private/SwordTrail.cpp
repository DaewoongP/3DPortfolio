#include "..\Public\SwordTrail.h"
#include "GameInstance.h"

CSwordTrail::CSwordTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CSwordTrail::CSwordTrail(const CSwordTrail& rhs)
	: CGameObject(rhs)
{
}

HRESULT CSwordTrail::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSwordTrail::Initialize(void* pArg)
{
	CTransform::TRANSFORMDESC TransformDesc = CTransform::TRANSFORMDESC(0.0, XMConvertToRadians(0.0f));
	if (FAILED(__super::Initialize(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;

	return S_OK;
}

void CSwordTrail::Tick(_double dTimeDelta)
{
	m_pTrailBufferCom->Tick();

	__super::Tick(dTimeDelta);
}

GAMEEVENT CSwordTrail::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	return GAME_NOEVENT;
}

HRESULT CSwordTrail::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;
	
	if (FAILED(m_pTrailBufferCom->Render("g_vColor", _float4(1.f, 0.1f, 0.1f, 0.3f), m_pShaderCom, 9)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSwordTrail::Add_Components(void* pArg)
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSG_BOX("Failed CSwordTrail Add_Component : (Com_Renderer)");
		return E_FAIL;
	}

	/* For.Com_TrailShader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSG_BOX("Failed CSwordTrail Add_Component : (Com_Shader)");
		return E_FAIL;
	}

	/* For.Com_TrailBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect_Trail"),
		TEXT("Com_TrailBuffer"), reinterpret_cast<CComponent**>(&m_pTrailBufferCom), pArg)))
	{
		MSG_BOX("Failed CPlayer Add_Component : (Com_TrailBuffer)");
		return E_FAIL;
	}

	/* For.Com_TrailTexture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Trail"),
		TEXT("Com_TrailTexture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX("Failed CPlayer Add_Component : (Com_TrailTexture)");
		return E_FAIL;
	}

	return S_OK;
}

void CSwordTrail::Add_Render()
{
	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
	}
}

CSwordTrail* CSwordTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSwordTrail* pInstance = new CSwordTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CSwordTrail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSwordTrail::Clone(void* pArg)
{
	CSwordTrail* pInstance = new CSwordTrail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CSwordTrail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSwordTrail::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTrailBufferCom);
}
