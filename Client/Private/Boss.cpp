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
	if (nullptr != pArg)
		m_EnemyDesc = *(static_cast<ENEMYDESC*>(pArg));
	else
	{
		MSG_BOX("Failed Read EnemyDesc");
		return E_FAIL;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(m_EnemyDesc)))
		return E_FAIL;

	if (FAILED(Add_Parts()))
		return E_FAIL;

	if (FAILED(SetUp_BehaviorTree()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(m_EnemyDesc.vScale);
	m_pTransformCom->Rotation(m_EnemyDesc.vRotation);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_EnemyDesc.vPosition));

	CTransform::TRANSFORMDESC TransformDesc = CTransform::TRANSFORMDESC(3.f, XMConvertToRadians(90.f));
	m_pTransformCom->Set_Desc(TransformDesc);
	// 네비게이션 초기위치 찾기.
	m_pNavigationCom->Find_MyCell(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_pModelCom->Reset_Animation(1);
	m_pModelCom->Delete_AnimationTranslation(5);

#ifdef _DEBUG
	m_pVisionColliderCom->Set_Color(DirectX::Colors::Aquamarine);
#endif // _DEBUG

	return S_OK;
}

void CBoss::Tick(_double dTimeDelta)
{
	/*__super::Tick(dTimeDelta);

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
	m_pVisionColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	pGameInstance->Add_Collider(COLLISIONDESC::COLTYPE_ENEMY, m_pColliderCom);
	pGameInstance->Add_Collider(COLLISIONDESC::COLTYPE_ENEMYVISION, m_pVisionColliderCom);

	Safe_Release(pGameInstance);*/
}

GAMEEVENT CBoss::Late_Tick(_double dTimeDelta)
{
//	AnimationState(dTimeDelta);
//
//	__super::Late_Tick(dTimeDelta);
//
//#ifdef _DEBUG
//	m_pRendererCom->Add_DebugGroup(m_pColliderCom);
//	m_pRendererCom->Add_DebugGroup(m_pVisionColliderCom);
//#endif // _DEBUG
//
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

HRESULT CBoss::Add_Component(ENEMYDESC& EnemyDesc)
{
	/* For.Com_Model */
	if (FAILED(CComposite::Add_Component(LEVEL_STAGE1, TEXT("Prototype_Component_Model_Enemy_Bakunin"),
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
	return E_NOTIMPL;
}

HRESULT CBoss::SetUp_BehaviorTree()
{
	return E_NOTIMPL;
}

HRESULT CBoss::SetUp_ShaderResources()
{
	return E_NOTIMPL;
}

void CBoss::AnimationState(_double dTimeDelta)
{
}

void CBoss::Motion_Change(ANIMATIONFLAG eAnimationFlag)
{
}

GAMEEVENT CBoss::PlayEvent(_double dTimeDelta)
{
	return GAMEEVENT();
}

CBoss* CBoss::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return nullptr;
}

CGameObject* CBoss::Clone(void* pArg)
{
	return nullptr;
}

void CBoss::Free()
{
}
