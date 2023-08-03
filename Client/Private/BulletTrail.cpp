#include "..\Public\BulletTrail.h"
#include "GameInstance.h"

CBulletTrail::CBulletTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CBulletTrail::CBulletTrail(const CBulletTrail& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBulletTrail::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBulletTrail::Initialize(void* pArg)
{
	CTransform::TRANSFORMDESC TransformDesc = CTransform::TRANSFORMDESC(0.0, XMConvertToRadians(0.0f));
	if (FAILED(__super::Initialize(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;

	return S_OK;
}

void CBulletTrail::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
}

GAMEEVENT CBulletTrail::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	return GAME_NOEVENT;
}

HRESULT CBulletTrail::Render()
{
	m_pTrailBufferCom->Tick();

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(m_pTrailBufferCom->Render("g_Texture", m_pTextureCom, m_pShaderCom, 7)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBulletTrail::Reset()
{
	if (FAILED(__super::Reset()))
		return E_FAIL;

	return S_OK;
}

void CBulletTrail::Render_Effect()
{
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

void CBulletTrail::Reset_Trail()
{
	m_pTrailBufferCom->Reset_Trail();
}

HRESULT CBulletTrail::Add_Components(void* pArg)
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSG_BOX("Failed CBulletTrail Add_Component : (Com_Renderer)");
		return E_FAIL;
	}

	/* For.Com_TrailShader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSG_BOX("Failed CBulletTrail Add_Component : (Com_Shader)");
		return E_FAIL;
	}

	/* For.Com_TrailBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect_Trail"),
		TEXT("Com_TrailBuffer"), reinterpret_cast<CComponent**>(&m_pTrailBufferCom), pArg)))
	{
		MSG_BOX("Failed CBulletTrail Add_Component : (Com_TrailBuffer)");
		return E_FAIL;
	}

	/* For.Com_TrailTexture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Large_Smoke_Trail"),
		TEXT("Com_TrailTexture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX("Failed CBulletTrail Add_Component : (Com_TrailTexture)");
		return E_FAIL;
	}

	return S_OK;
}

CBulletTrail* CBulletTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBulletTrail* pInstance = new CBulletTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBulletTrail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBulletTrail::Clone(void* pArg)
{
	CBulletTrail* pInstance = new CBulletTrail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CBulletTrail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBulletTrail::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTrailBufferCom);
}
