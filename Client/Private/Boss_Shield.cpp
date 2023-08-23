#include "..\Public\Boss_Shield.h"
#include "GameInstance.h"
#include "Boss.h"

CBoss_Shield::CBoss_Shield(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CBoss_Shield::CBoss_Shield(const CBoss_Shield& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBoss_Shield::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Shield::Initialize(void* pArg)
{
	CTransform::TRANSFORMDESC TransformDesc = CTransform::TRANSFORMDESC(0.0, XMConvertToRadians(0.0f));
	if (FAILED(__super::Initialize(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_vEmissiveColor = _float4(1.f, 0.1f, 0.1f, 1.f);
	
	m_pTransformCom->Set_Scale(_float3(10.f, 10.f, 10.f));

	return S_OK;
}

void CBoss_Shield::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
	
	m_pTransformCom->Go_Straight(dTimeDelta);

	m_pModelCom->Play_Animation(dTimeDelta);
}

GAMEEVENT CBoss_Shield::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	m_dRenderTimeAcc += dTimeDelta;

	if (nullptr != m_pRendererCom &&
		m_dRenderTimeAcc < m_dRenderTime)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
	}

	return GAME_NOEVENT;
}

HRESULT CBoss_Shield::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE);

		m_pShaderCom->Begin(2);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CBoss_Shield::Add_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSG_BOX("Failed CBoss_Shield Add_Component : (Com_Renderer)");
		return E_FAIL;
	}

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_BOSS, TEXT("Prototype_Component_Model_Bakunin_Shield"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSG_BOX("Failed CBoss_Shield Add_Component : (Com_Model)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSG_BOX("Failed CBoss_Shield Add_Component : (Com_Shader)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBoss_Shield::SetUp_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", pGameInstance->Get_CamFar(), sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vEmissive", &m_vEmissiveColor, sizeof(_float4))))
		return E_FAIL;

	_bool isNormal = false;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_isNormal", &isNormal, sizeof(_bool))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

void CBoss_Shield::Add_Render(_vector vLook, _vector vPos, _double dRenderTime)
{
	m_dRenderTime = dRenderTime;
	m_dRenderTimeAcc = 0.0;
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos + XMVectorSet(0.f, 2.f, 0.f, 0.f) + vLook * 2.f);
}

CBoss_Shield* CBoss_Shield::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBoss_Shield* pInstance = new CBoss_Shield(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBoss_Shield");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBoss_Shield::Clone(void* pArg)
{
	CBoss_Shield* pInstance = new CBoss_Shield(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CBoss_Shield");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_Shield::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
