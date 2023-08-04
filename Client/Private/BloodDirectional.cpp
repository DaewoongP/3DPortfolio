#include "..\Public\BloodDirectional.h"
#include "GameInstance.h"

CBloodDirectional::CBloodDirectional(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CBloodDirectional::CBloodDirectional(const CBloodDirectional& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBloodDirectional::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBloodDirectional::Initialize(void* pArg)
{
	CTransform::TRANSFORMDESC TransformDesc;
	ZEROMEM(&TransformDesc);

	if (FAILED(__super::Initialize(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_iRandTextureNum = rand() % 6;
	m_vColor = _float4(1.f, 0.f, 0.f, 1.f);

	return S_OK;
}

void CBloodDirectional::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixScaling(6.f, 6.f, 6.f));
	m_pVIBufferCom->Tick(&WorldMatrix);
}

GAMEEVENT CBloodDirectional::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	m_dRenderTimeAcc += dTimeDelta;

	if (m_dRenderTimeAcc < m_dRenderTime)
	{
		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
	}
	
	return GAME_NOEVENT;
}

HRESULT CBloodDirectional::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(2);

	m_pVIBufferCom->Render();

	return S_OK;
}

void CBloodDirectional::Render_Effect(_double dRenderTime, _fvector vEffectPos)
{
	m_dRenderTimeAcc = 0.0;
	m_dRenderTime = dRenderTime;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vEffectPos);
}

HRESULT CBloodDirectional::Add_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	m_iParticleNum = { 1 };
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point_Instance"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom, &m_iParticleNum)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Blood_Directional"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBloodDirectional::SetUp_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iRandTextureNum)))
		return E_FAIL;

	m_vColor.w -= (_float)g_TimeDelta;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

CBloodDirectional* CBloodDirectional::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBloodDirectional* pInstance = new CBloodDirectional(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBloodDirectional");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBloodDirectional::Clone(void* pArg)
{
	CBloodDirectional* pInstance = new CBloodDirectional(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CBloodDirectional");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBloodDirectional::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}
