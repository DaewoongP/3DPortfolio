#include "../Public/Shuriken.h"
#include "GameInstance.h"
#include "Player.h"

CShuriken::CShuriken(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPart(pDevice, pContext)
{
}

CShuriken::CShuriken(const CShuriken& rhs)
	: CPart(rhs)
{
}

HRESULT CShuriken::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CShuriken::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Initialize_ParentMatrix(*reinterpret_cast<PARENTMATRIXDESC*>(pArg))))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CShuriken::Initialize_Level(_uint iLevelIndex)
{
	if (FAILED(Add_Components_Level(iLevelIndex)))
		return E_FAIL;

	m_dSpeed = 100.0;
	CTransform::TRANSFORMDESC TransformDesc = CTransform::TRANSFORMDESC(m_dSpeed, XMConvertToRadians(0.0f));
	m_pTransformCom->Set_Desc(TransformDesc);

	return S_OK;
}

HRESULT CShuriken::Initialize_ParentMatrix(PARENTMATRIXDESC ParentDesc)
{
	m_ParentMatrixDesc = ParentDesc;

	return S_OK;
}

void CShuriken::Tick(_double dTimeDelta)
{
	m_dTimeAcc += dTimeDelta;

	m_pModelCom->Play_Animation(dTimeDelta);

	if (0.6 < m_dTimeAcc)
		AttackEnd();

	__super::Tick(dTimeDelta);
}

GAMEEVENT CShuriken::Late_Tick(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (0.2 < m_dTimeAcc &&
		true == m_isAttacked)
	{
		_vector vDir = XMLoadFloat3(&m_vAttackDir) * (_float)m_dSpeed * (_float)dTimeDelta;
		XMStoreFloat4x4(&m_CombinedWorldMatrix,
			XMLoadFloat4x4(&m_CombinedWorldMatrix) * XMMatrixTranslation(XMVectorGetX(vDir), XMVectorGetY(vDir), XMVectorGetZ(vDir)));
		pGameInstance->Add_Collider(COLLISIONDESC::COLTYPE_PLAYERWEAPON, m_pColliderCom);
	}
	else
		__super::Late_Tick(dTimeDelta);

	Safe_Release(pGameInstance);
	
	m_pColliderCom->Tick(XMLoadFloat4x4(&m_CombinedWorldMatrix));

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
#ifdef _DEBUG
		m_pRendererCom->Add_DebugGroup(m_pColliderCom);
#endif // _DEBUG
	}

	return GAME_NOEVENT;
}

HRESULT CShuriken::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE);
		m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TextureType_NORMALS);

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CShuriken::Add_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSG_BOX("Failed CShuriken Add_Component : (Com_Renderer)");
		return E_FAIL;
	}

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Shuriken"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSG_BOX("Failed CShuriken Add_Component : (Com_Model)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSG_BOX("Failed CShuriken Add_Component : (Com_Shader)");
		return E_FAIL;
	}

	CBounding_Sphere::BOUNDINGSPHEREDESC SphereDesc;
	SphereDesc.fRadius = 0.2f;
	SphereDesc.vPosition = _float3(0.f, 0.f, 0.f);
	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &SphereDesc)))
	{
		MSG_BOX("Failed CShuriken Add_Component : (Com_Collider)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CShuriken::Add_Components_Level(_uint iLevelIndex)
{
	

	return S_OK;
}

HRESULT CShuriken::SetUp_ShaderResources()
{
	// Part의 경우 월드행렬을 다 저장된거로 던져야함.
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix",
		pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix",
		pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", pGameInstance->Get_CamFar(), sizeof(_float))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

void CShuriken::Attack(_fvector vDirection)
{
	if (true == m_isAttacked)
		return;

	m_dTimeAcc = 0.0;
	m_isAttacked = true;
	XMStoreFloat3(&m_vAttackDir, vDirection);
}

void CShuriken::AttackEnd()
{
	if (false == m_isAttacked)
		return;

	m_isAttacked = false;
	m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());
}

void CShuriken::Reset_Collider()
{
	
}

CShuriken* CShuriken::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CShuriken* pInstance = new CShuriken(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CShuriken");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CShuriken::Clone(void* pArg)
{
	CShuriken* pInstance = new CShuriken(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CShuriken");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShuriken::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pColliderCom);
}