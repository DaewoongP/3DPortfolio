#include "..\Public\Enemy_Hammer.h"
#include "GameInstance.h"
#include "Hammer.h"
#include "Selector_Hammer.h"

CEnemy_Hammer::CEnemy_Hammer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEnemy(pDevice, pContext)
{
}

CEnemy_Hammer::CEnemy_Hammer(const CEnemy_Hammer& rhs)
	: CEnemy(rhs)
{
}

HRESULT CEnemy_Hammer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnemy_Hammer::Initialize(void* pArg)
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

	if (FAILED(Add_Component()))
		return E_FAIL;

	if (FAILED(SetUp_BehaviorTree()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnemy_Hammer::Initialize_Level(_uint iLevelIndex)
{
	if (FAILED(Add_Parts(iLevelIndex)))
		return E_FAIL;

	m_pTransformCom->Set_Scale(m_EnemyDesc.vScale);
	m_pTransformCom->Rotation(m_EnemyDesc.vRotation);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_EnemyDesc.vPosition));

	CTransform::TRANSFORMDESC TransformDesc = CTransform::TRANSFORMDESC(1.f, XMConvertToRadians(360.f));
	m_pTransformCom->Set_Desc(TransformDesc);
	// 네비게이션 초기위치 찾기.
	m_pNavigationCom->Find_MyCell(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_pModelCom->Reset_Animation(52);
	m_pModelCom->Delete_AnimationTranslation(68);

#ifdef _DEBUG
	m_pVisionColliderCom->Set_Color(DirectX::Colors::Aquamarine);
#endif // _DEBUG

	return S_OK;
}

void CEnemy_Hammer::Tick(_double dTimeDelta)
{
	Attack();

	__super::Tick(dTimeDelta);

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
	m_pVisionColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	pGameInstance->Add_Collider(COLLISIONDESC::COLTYPE_ENEMY, m_pColliderCom);
	pGameInstance->Add_Collider(COLLISIONDESC::COLTYPE_ENEMYVISION, m_pVisionColliderCom);

	Safe_Release(pGameInstance);
}

GAMEEVENT CEnemy_Hammer::Late_Tick(_double dTimeDelta)
{
	AnimationState(dTimeDelta);

	__super::Late_Tick(dTimeDelta);


#ifdef _DEBUG
	m_pRendererCom->Add_DebugGroup(m_pColliderCom);
	m_pRendererCom->Add_DebugGroup(m_pVisionColliderCom);
#endif // _DEBUG

	return PlayEvent(dTimeDelta);
}

void CEnemy_Hammer::OnCollisionEnter(COLLISIONDESC CollisionDesc)
{
	if (CollisionDesc.pMyCollider == m_pColliderCom &&
		COLLISIONDESC::COLTYPE_PLAYERWEAPON == CollisionDesc.ColType)
	{
		m_eGameEvent = GAME_OBJECT_DEAD;
	}
}

void CEnemy_Hammer::OnCollisionStay(COLLISIONDESC CollisionDesc)
{
	if (COLLISIONDESC::COLTYPE_PLAYER == CollisionDesc.ColType)
		m_pTargetPlayer = CollisionDesc.pOtherOwner;

	__super::OnCollisionStay(CollisionDesc);
}

void CEnemy_Hammer::OnCollisionExit(COLLISIONDESC CollisionDesc)
{
	if (COLLISIONDESC::COLTYPE_PLAYER == CollisionDesc.ColType)
		m_pTargetPlayer = nullptr;
}

HRESULT CEnemy_Hammer::Render()
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

HRESULT CEnemy_Hammer::Reset()
{
	m_pModelCom->Reset_Animation(52);

	// 상태값 통일하면 애니메이션 초기화도 간단함.
	m_ePreState = STATE_IDLE;
	m_eCurState = STATE_IDLE;

	m_pTargetPlayer = nullptr;

	if (FAILED(__super::Reset()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnemy_Hammer::Add_Component()
{
	/* For.Com_Model */
	if (FAILED(CComposite::Add_Component(LEVEL_STAGE1, TEXT("Prototype_Component_Model_Enemy_Hammer"),
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

HRESULT CEnemy_Hammer::Add_Parts(_uint iLevelIndex)
{
	CPart::PARENTMATRIXDESC ParentMatrixDesc;
	ZEROMEM(&ParentMatrixDesc);

	const CBone* pBone = m_pModelCom->Get_Bone(TEXT("Gun_r"));

	ParentMatrixDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	ParentMatrixDesc.OffsetMatrix = pBone->Get_OffsetMatrix();
	ParentMatrixDesc.pCombindTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
	ParentMatrixDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();

	if (FAILED(__super::Add_Part(iLevelIndex, TEXT("Prototype_GameObject_Weapon_Hammer"), TEXT("Layer_EnemyWeapon"),
		TEXT("Part_Hammer"), reinterpret_cast<CGameObject**>(&m_pHammer), &ParentMatrixDesc)))
	{
		MSG_BOX("Failed Create Hammer");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEnemy_Hammer::SetUp_BehaviorTree()
{
	CBlackBoard* pBlackBoard = CBlackBoard::Create();
	pBlackBoard->Add_Value(TEXT("Value_isDead"), &m_isDead);
	pBlackBoard->Add_Value(TEXT("Value_Target"), &m_pTargetPlayer);
	/* Sequence Patrol */
	pBlackBoard->Add_Value(TEXT("Value_Transform"), m_pTransformCom);
	pBlackBoard->Add_Value(TEXT("Value_Navigation"), m_pNavigationCom);
	pBlackBoard->Add_Value(TEXT("Value_isWalk"), &m_isWalk);
	pBlackBoard->Add_Value(TEXT("Value_isWait"), &m_isWait);
	m_dMaxWaitTime = 5.0;
	pBlackBoard->Add_Value(TEXT("Value_MaxWaitTime"), &m_dMaxWaitTime);
	/* Sequence SlowLookAttack */
	pBlackBoard->Add_Value(TEXT("Value_isReady"), &m_isReady);
	m_dReadyTime = 0.5;
	pBlackBoard->Add_Value(TEXT("Value_ReadyTime"), &m_dReadyTime);
	m_dAttackCoolTime = 0.0;
	pBlackBoard->Add_Value(TEXT("Value_AttackCoolTime"), &m_dAttackCoolTime);
	pBlackBoard->Add_Value(TEXT("Value_isAttack"), &m_isAttack);
	pBlackBoard->Add_Value(TEXT("Value_Weapon"), m_pHammer);
	m_dWaitTime = 2.0;
	pBlackBoard->Add_Value(TEXT("Value_WaitTime"), &m_dWaitTime);


	/* For. Com_BehaviorTree */
	if (FAILED(CComposite::Add_Component(LEVEL_STAGE1, TEXT("Prototype_Component_BehaviorTree"),
		TEXT("Com_BehaviorTree"), reinterpret_cast<CComponent**>(&m_pBehaviorTreeCom),
		CSelector_Hammer::Create(pBlackBoard))))
	{
		MSG_BOX("Failed CEnemy_Pistol Add_Component : (Com_BehaviorTree)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEnemy_Hammer::SetUp_ShaderResources()
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

void CEnemy_Hammer::AnimationState(_double dTimeDelta)
{
	m_eCurrentAnimationFlag = m_pModelCom->Get_AnimationState();

	// 애니메이션이 종료 되었을때 따로 처리되는 명령이 없을 경우 IDLE처리
	if (ANIM_FINISHED == m_eCurrentAnimationFlag &&
		m_ePreState == m_eCurState)
	{
		m_isAttack = false;
		m_eCurState = STATE_IDLE;

		if (STATE_ATTACK == m_eCurState)
			m_eCurState = STATE_WAIT;
	}

	if (STATE_DEAD != m_eCurState)
	{
		if (true == m_isWait)
		{
			m_eCurState = STATE_WAIT;
		}
		if (true == m_isWalk)
			m_eCurState = STATE_WALK;
		if (true == m_isReady)
			m_eCurState = STATE_READY;
		if (true == m_isAttack)
			m_eCurState = STATE_ATTACK;
	}

	Motion_Change(m_eCurrentAnimationFlag);

	m_pModelCom->Play_Animation(dTimeDelta);
}

void CEnemy_Hammer::Motion_Change(ANIMATIONFLAG eAnimationFlag)
{
	if (false == (ANIM_PLAYING == eAnimationFlag || ANIM_FINISHED == eAnimationFlag))
		return;

	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case STATE_IDLE:
			m_pModelCom->Set_AnimIndex(52);
			break;
		case STATE_WALK:
			m_pModelCom->Set_AnimIndex(68);
			break;
		case STATE_READY:
			m_pModelCom->Set_AnimIndex(53, false);
			break;
		case STATE_ATTACK:
			m_pModelCom->Set_AnimIndex(62, false);
			break;
		case STATE_WAIT:
			m_pModelCom->Set_AnimIndex(50, false);
			break;
		case STATE_DEAD:
			m_pModelCom->Set_AnimIndex(44 + rand() % 6, false);
			break;
		}

		m_ePreState = m_eCurState;
	}
}

void CEnemy_Hammer::Attack()
{
	if (STATE_ATTACK != m_eCurState)
		return;

	_float fCurrentFramePercent = m_pModelCom->Get_CurrentFramePercent();

	if (0.3f < fCurrentFramePercent)
		m_pHammer->Attack();
}

GAMEEVENT CEnemy_Hammer::PlayEvent(_double dTimeDelta)
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

CEnemy_Hammer* CEnemy_Hammer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEnemy_Hammer* pInstance = new CEnemy_Hammer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CEnemy_Hammer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEnemy_Hammer::Clone(void* pArg)
{
	CEnemy_Hammer* pInstance = new CEnemy_Hammer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CEnemy_Hammer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEnemy_Hammer::Free()
{
	__super::Free();

	Safe_Release(m_pHammer);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pVisionColliderCom);
	Safe_Release(m_pBehaviorTreeCom);
}
