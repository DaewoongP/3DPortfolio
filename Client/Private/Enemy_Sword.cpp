#include "..\Public\Enemy_Sword.h"
#include "GameInstance.h"
#include "Selector_FindTargetToAttack.h"
#include "Sword.h"

CEnemy_Sword::CEnemy_Sword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEnemy(pDevice, pContext)
{
}

CEnemy_Sword::CEnemy_Sword(const CEnemy_Sword& rhs)
	: CEnemy(rhs)
{
}

HRESULT CEnemy_Sword::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnemy_Sword::Initialize(void* pArg)
{
	ENEMYDESC EnemyDesc;
	ZEROMEM(&EnemyDesc);

	if (nullptr != pArg)
		EnemyDesc = *(static_cast<ENEMYDESC*>(pArg));
	else
	{
		MSG_BOX("Failed Read EnemyDesc");
		return E_FAIL;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(EnemyDesc)))
		return E_FAIL;

	if (FAILED(Add_Parts()))
		return E_FAIL;

	if (FAILED(SetUp_BehaviorTree()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(EnemyDesc.vScale);
	m_pTransformCom->Rotation(EnemyDesc.vRotation);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&EnemyDesc.vPosition));

	CTransform::TRANSFORMDESC TransformDesc = CTransform::TRANSFORMDESC(3.f, XMConvertToRadians(90.f));
	m_pTransformCom->Set_Desc(TransformDesc);
	// 네비게이션 초기위치 찾기.
	m_pNavigationCom->Find_MyCell(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_pModelCom->Reset_Animation(1);
	m_pModelCom->Delete_AnimationTranslation(5);

	return S_OK;
}

void CEnemy_Sword::Tick(_double dTimeDelta)
{
	AnimationState(dTimeDelta);

	__super::Tick(dTimeDelta);

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
	m_pVisionColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	pGameInstance->Add_Collider(COLLISIONDESC::COLTYPE_ENEMY, m_pColliderCom);
	pGameInstance->Add_Collider(COLLISIONDESC::COLTYPE_ENEMYVISION, m_pVisionColliderCom);

	Safe_Release(pGameInstance);
}

GAMEEVENT CEnemy_Sword::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	return PlayEvent(dTimeDelta);
}

void CEnemy_Sword::OnCollisionEnter(COLLISIONDESC CollisionDesc)
{
	if (CollisionDesc.pMyCollider == m_pColliderCom &&
		!lstrcmp(CollisionDesc.pOtherOwner->Get_LayerTag(), TEXT("Layer_PlayerWeapon")))
	{
		m_eGameEvent = GAME_OBJECT_DEAD;
	}
}

void CEnemy_Sword::OnCollisionStay(COLLISIONDESC CollisionDesc)
{
	if (COLLISIONDESC::COLTYPE_PLAYER == CollisionDesc.ColType)
		m_pTargetPlayer = CollisionDesc.pOtherOwner;
}

void CEnemy_Sword::OnCollisionExit(COLLISIONDESC CollisionDesc)
{
	if (COLLISIONDESC::COLTYPE_PLAYER == CollisionDesc.ColType)
		m_pTargetPlayer = nullptr;
}

HRESULT CEnemy_Sword::Render()
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
		// m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS);

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

#ifdef _DEBUG
	m_pColliderCom->Render();
	m_pVisionColliderCom->Render(DirectX::Colors::Aquamarine);
#endif // _DEBUG

	return S_OK;
}

HRESULT CEnemy_Sword::Reset()
{
	m_pModelCom->Reset_Animation(1);
	m_eCurState = STATE_IDLE;

	if (FAILED(__super::Reset()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnemy_Sword::Add_Component(ENEMYDESC& EnemyDesc)
{
	/* For.Com_Model */
	if (FAILED(CComposite::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Enemy_Sword"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		MSG_BOX("Failed CEnemy_Pistol Add_Component : (Com_Model)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		MSG_BOX("Failed CEnemy_Pistol Add_Component : (Com_Shader)");
		return E_FAIL;
	}

	CBounding_AABB::BOUNDINGAABBDESC AABBDesc;

	AABBDesc.vExtents = _float3(1.f, 2.4f, 1.f);
	AABBDesc.vPosition = _float3(0.f, AABBDesc.vExtents.y, 0.f);

	/* For.Com_Collider */
	if (FAILED(CComposite::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &AABBDesc)))
	{
		MSG_BOX("Failed CEnemy_Pistol Add_Component : (Com_Collider)");
		return E_FAIL;
	}

	CBounding_Sphere::BOUNDINGSPHEREDESC SphereDesc;

	SphereDesc.fRadius = 20.f;
	SphereDesc.vPosition = _float3(0.f, 0.f, 0.f);

	/* For.Com_VisionCollider */
	if (FAILED(CComposite::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_VisionCollider"), reinterpret_cast<CComponent**>(&m_pVisionColliderCom), &SphereDesc)))
	{
		MSG_BOX("Failed CEnemy_Pistol Add_Component : (Com_VisionCollider)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEnemy_Sword::Add_Parts()
{
	CPart::PARENTMATRIXDESC ParentMatrixDesc;
	ZEROMEM(&ParentMatrixDesc);

	const CBone* pBone = m_pModelCom->Get_Bone(TEXT("Gun_r"));

	ParentMatrixDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	ParentMatrixDesc.OffsetMatrix = pBone->Get_OffsetMatrix();
	ParentMatrixDesc.pCombindTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
	ParentMatrixDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();

	if (FAILED(__super::Add_Part(TEXT("Prototype_GameObject_Weapon_Sword"), TEXT("Layer_EnemyPart"),
		TEXT("Part_Sword"), reinterpret_cast<CGameObject**>(&m_pSword), &ParentMatrixDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnemy_Sword::SetUp_BehaviorTree()
{
	CBlackBoard* pBlackBoard = CBlackBoard::Create();
	pBlackBoard->Add_Value(TEXT("Value_Transform"), m_pTransformCom);
	pBlackBoard->Add_Value(TEXT("Value_Navigation"), m_pNavigationCom);
	pBlackBoard->Add_Value(TEXT("Value_Target"), &m_pTargetPlayer);
	pBlackBoard->Add_Value(TEXT("Value_isWalk"), &m_isWalk);
	pBlackBoard->Add_Value(TEXT("Value_isDead"), &m_isDead);

	m_dPatrolWaitTime = 8.0;
	pBlackBoard->Add_Value(TEXT("Value_MaxWaitTime"), &m_dPatrolWaitTime);
	pBlackBoard->Add_Value(TEXT("Value_isWait"), &m_isWait);

	m_dAttackCoolTime = 1.5;
	pBlackBoard->Add_Value(TEXT("Value_AttackCoolTime"), &m_dAttackCoolTime);
	pBlackBoard->Add_Value(TEXT("Value_isAttack"), &m_isAttack);
	pBlackBoard->Add_Value(TEXT("Value_Weapon"), m_pSword);

	/* For. Com_BehaviorTree */
	if (FAILED(CComposite::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_BehaviorTree"),
		TEXT("Com_BehaviorTree"), reinterpret_cast<CComponent**>(&m_pBehaviorTreeCom),
		CSelector_FindTargetToAttack::Create(pBlackBoard))))
	{
		MSG_BOX("Failed CEnemy_Pistol Add_Component : (Com_BehaviorTree)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEnemy_Sword::SetUp_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

void CEnemy_Sword::AnimationState(_double dTimeDelta)
{
	m_eCurrentAnimationFlag = m_pModelCom->Get_AnimationState();

	// 애니메이션이 종료 되었을때 따로 처리되는 명령이 없을 경우 IDLE처리
	if (ANIM_FINISHED == m_eCurrentAnimationFlag &&
		m_ePreState == m_eCurState)
	{
		m_isAttack = false;

		m_eCurState = STATE_IDLE;
	}

	if (STATE_DEAD != m_eCurState)
	{
		if (true == m_isWait)
			m_eCurState = STATE_IDLE;
		if (true == m_isWalk)
			m_eCurState = STATE_WALK;
		if (true == m_isAttack)
			m_eCurState = STATE_READY;
	}

	Motion_Change(m_eCurrentAnimationFlag);

	m_pModelCom->Play_Animation(dTimeDelta);
}

void CEnemy_Sword::Motion_Change(ANIMATIONFLAG eAnimationFlag)
{
	if (false == (ANIM_PLAYING == eAnimationFlag || ANIM_FINISHED == eAnimationFlag))
		return;

	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case STATE_IDLE:
			m_pModelCom->Set_AnimIndex(1);
			break;
		case STATE_READY:
			m_pModelCom->Set_AnimIndex(19, false);
			break;
		case STATE_DASH:
			m_pModelCom->Set_AnimIndex(20, false);
			break;
		case STATE_ATTACK:
			m_pModelCom->Set_AnimIndex(21, false);
			break;
		case STATE_WALK:
			m_pModelCom->Set_AnimIndex(5);
			break;
		case STATE_DEAD:
			m_pModelCom->Set_AnimIndex(13 + rand() % 3, false);
			break;
		}

		m_ePreState = m_eCurState;
	}
}

GAMEEVENT CEnemy_Sword::PlayEvent(_double dTimeDelta)
{
	if (GAME_OBJECT_DEAD == m_eGameEvent)
	{
		m_eCurState = STATE_DEAD;
		m_isDead = true;

		if (ANIM_FINISHED == m_eCurrentAnimationFlag)
			return GAME_OBJECT_DEAD;
	}

	return GAME_NOEVENT;
}

CEnemy_Sword* CEnemy_Sword::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEnemy_Sword* pInstance = new CEnemy_Sword(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CEnemy_Sword");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEnemy_Sword::Clone(void* pArg)
{
	CEnemy_Sword* pInstance = new CEnemy_Sword(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CEnemy_Sword");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEnemy_Sword::Free()
{
	__super::Free();

	Safe_Release(m_pSword);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pVisionColliderCom);
	Safe_Release(m_pBehaviorTreeCom);
}
