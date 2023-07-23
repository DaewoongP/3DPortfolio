#include "..\Public\Boss.h"
#include "GameInstance.h"

CBoss::CBoss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEnemy(pDevice, pContext)
{
}

CBoss::CBoss(const CBoss& rhs)
	: CEnemy(rhs)
{
}

HRESULT CBoss::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	if (FAILED(Add_Parts()))
		return E_FAIL;

	if (FAILED(SetUp_BehaviorTree()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(_float3(1.f, 1.f, 1.f));
	m_pTransformCom->Rotation(_float3(0.f, 0.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(10.f, 0.f, 10.f, 1.f));

	CTransform::TRANSFORMDESC TransformDesc = CTransform::TRANSFORMDESC(3.f, XMConvertToRadians(90.f));
	m_pTransformCom->Set_Desc(TransformDesc);

	m_pModelCom->Reset_Animation(1);
	m_pModelCom->Delete_AnimationTranslation(5);

#ifdef _DEBUG
	m_pVisionColliderCom->Set_Color(DirectX::Colors::Aquamarine);
#endif // _DEBUG

	return S_OK;
}

void CBoss::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
	m_pVisionColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	pGameInstance->Add_Collider(COLLISIONDESC::COLTYPE_ENEMY, m_pColliderCom);
	pGameInstance->Add_Collider(COLLISIONDESC::COLTYPE_ENEMYVISION, m_pVisionColliderCom);

	Safe_Release(pGameInstance);
}

GAMEEVENT CBoss::Late_Tick(_double dTimeDelta)
{
	AnimationState(dTimeDelta);

	__super::Late_Tick(dTimeDelta);

	if (nullptr != m_pRendererCom)
	{
#ifdef _DEBUG
		m_pRendererCom->Add_DebugGroup(m_pColliderCom);
		m_pRendererCom->Add_DebugGroup(m_pVisionColliderCom);
#endif // _DEBUG
	}

	return PlayEvent(dTimeDelta);
}

void CBoss::OnCollisionEnter(COLLISIONDESC CollisionDesc)
{
}

void CBoss::OnCollisionStay(COLLISIONDESC CollisionDesc)
{
}

void CBoss::OnCollisionExit(COLLISIONDESC CollisionDesc)
{
}

HRESULT CBoss::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE);
		m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TextureType_NORMALS);

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CBoss::Reset()
{
	//m_pModelCom->Reset_Animation(1);

	// 상태값 통일하면 애니메이션 초기화도 간단함.
	m_ePreState = STATE_IDLE;
	m_eCurState = STATE_IDLE;

	m_pTargetPlayer = nullptr;

	if (FAILED(__super::Reset()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss::Add_Component()
{
	/* For.Com_Model */
	if (FAILED(CComposite::Add_Component(LEVEL_BOSS, TEXT("Prototype_Component_Model_Enemy_Bakunin"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		MSG_BOX("Failed CEnemy_Hammer Add_Component : (Com_Model)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		MSG_BOX("Failed CEnemy_Hammer Add_Component : (Com_Shader)");
		return E_FAIL;
	}

	CBounding_AABB::BOUNDINGAABBDESC AABBDesc;

	AABBDesc.vExtents = _float3(1.f, 2.4f, 1.f);
	AABBDesc.vPosition = _float3(0.f, AABBDesc.vExtents.y, 0.f);

	/* For.Com_Collider */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &AABBDesc)))
	{
		MSG_BOX("Failed CEnemy_Hammer Add_Component : (Com_Collider)");
		return E_FAIL;
	}

	CBounding_Sphere::BOUNDINGSPHEREDESC SphereDesc;

	SphereDesc.fRadius = 10.f;
	SphereDesc.vPosition = _float3(0.f, 0.f, 0.f);

	/* For.Com_VisionCollider */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_VisionCollider"), reinterpret_cast<CComponent**>(&m_pVisionColliderCom), &SphereDesc)))
	{
		MSG_BOX("Failed CEnemy_Hammer Add_Component : (Com_VisionCollider)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBoss::Add_Parts()
{
	return S_OK;
}

HRESULT CBoss::SetUp_BehaviorTree()
{
	return S_OK;
}

HRESULT CBoss::SetUp_ShaderResources()
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

	Safe_Release(pGameInstance);

	return S_OK;
}

void CBoss::AnimationState(_double dTimeDelta)
{
}

void CBoss::Motion_Change(ANIMATIONFLAG eAnimationFlag)
{
}

GAMEEVENT CBoss::PlayEvent(_double dTimeDelta)
{
	return GAME_NOEVENT;
}

CBoss* CBoss::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBoss* pInstance = new CBoss(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBoss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBoss::Clone(void* pArg)
{
	CBoss* pInstance = new CBoss(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CBoss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pVisionColliderCom);
	Safe_Release(m_pBehaviorTreeCom);
}
