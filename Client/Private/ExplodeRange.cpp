#include "..\Public\ExplodeRange.h"
#include "GameInstance.h"

CExplodeRange::CExplodeRange(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

HRESULT CExplodeRange::Initialize(_fvector vPos, _float fRange, _double dExplodeTime)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	CTransform::TRANSFORMDESC TransDesc;
	ZEROMEM(&TransDesc);
	
	if (FAILED(__super::Initialize(nullptr, &TransDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(_float3(fRange, fRange, 1.f));
	m_pTransformCom->Rotation(_float3(90.f, 0.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_fRange = fRange;
	m_dExplodeTime = dExplodeTime;

	return S_OK;
}

void CExplodeRange::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	m_dExplodeTimeAcc += dTimeDelta;
}

GAMEEVENT CExplodeRange::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (m_dExplodeTimeAcc > 3.0)
		return GAME_OBJECT_DEAD;

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);

	return GAME_NOEVENT;
}

HRESULT CExplodeRange::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(6);

	m_pBufferCom->Render();

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CExplodeRange::Add_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		MSG_BOX("Failed CEnemy Add_Component : (Com_Renderer)");
		return E_FAIL;
	}

	/* For.Com_ExplodeShader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TEXT("Com_ExplodeShader"), (CComponent**)&m_pShaderCom)))
	{
		MSG_BOX("Failed CBoss_Sword Add_Component : (Com_ExplodeShader)");
		return E_FAIL;
	}
	/* For.Com_Texture */
	if (FAILED(CComposite::Add_Component(LEVEL_BOSS, TEXT("Prototype_Component_Texture_Bomb_Explode"),
		TEXT("Com_ExplodeTexture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX("Failed CBoss Add_Component : (Com_ExplodeTexture)");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pBufferCom))))
	{
		MSG_BOX("Failed BackGround Add_Component : (Com_VIBuffer)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CExplodeRange::SetUp_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fSize", &m_fRange, sizeof(_float))))
		return E_FAIL;
	_float fTimeAcc = _float(m_dExplodeTimeAcc);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeAcc", &fTimeAcc, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CExplodeRange* CExplodeRange::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _fvector vPos, _float fRange, _double dExplodeTime)
{
	CExplodeRange* pInstance = new CExplodeRange(pDevice, pContext);

	if (FAILED(pInstance->Initialize(vPos, fRange, dExplodeTime)))
	{
		MSG_BOX("Failed to Created CExplodeRange");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CExplodeRange::Free()
{
	__super::Free();

	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
}
