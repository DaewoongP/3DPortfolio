#include "..\Public\Boss.h"
#include "GameInstance.h"
#include "Boss_Sword.h"
#include "Selector_Boss.h"
#include "Bomb.h"

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
	Tick_Bomb(dTimeDelta);

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
	Late_Tick_Bomb(dTimeDelta);

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
	if (CollisionDesc.pMyCollider == m_pColliderCom &&
		COLLISIONDESC::COLTYPE_PLAYERWEAPON == CollisionDesc.ColType)
	{
		// 플레이어 칼 충돌
	}
}

void CBoss::OnCollisionStay(COLLISIONDESC CollisionDesc)
{
	// 시야범위 발견
	if (COLLISIONDESC::COLTYPE_PLAYER == CollisionDesc.ColType)
		m_pTargetPlayer = CollisionDesc.pOtherOwner;

	// 서로 밀려나는건데 안해도될듯
	//__super::OnCollisionStay(CollisionDesc);
}

void CBoss::OnCollisionExit(COLLISIONDESC CollisionDesc)
{
	if (COLLISIONDESC::COLTYPE_PLAYER == CollisionDesc.ColType)
		m_pTargetPlayer = nullptr;
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
	m_pModelCom->Reset_Animation(1);

	// 상태값 통일하면 애니메이션 초기화도 간단함.
	m_ePreState = STATE_IDLE;
	m_eCurState = STATE_IDLE;

	m_pTargetPlayer = nullptr;

	for (auto& pBomb : m_Bombs)
		pBomb->Reset();

	if (FAILED(__super::Reset()))
		return E_FAIL;

	return S_OK;
}

void CBoss::Throw_Bomb(_fvector vTargetPosition)
{
	CGameObject* pBomb = { nullptr };

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CGameObject* pGameObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Bomb"), nullptr);

	pGameObject->Set_Owner(this);

	pGameObject->Set_LayerTag(TEXT("Layer_EnemyWeapon"));

	pGameObject->Set_Tag(TEXT("GameObject_Bomb"));

	pGameObject->Initialize_Level(LEVEL_BOSS);

	m_Bombs.push_back(pGameObject);

	Safe_Release(pGameInstance);

	static_cast<CBomb*>(pGameObject)->Fire(m_pTransformCom->Get_State(CTransform::STATE_POSITION), vTargetPosition);
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

	SphereDesc.fRadius = 200.f;
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
	CPart::PARENTMATRIXDESC ParentMatrixDesc;
	ZEROMEM(&ParentMatrixDesc);

	const CBone* pBone = m_pModelCom->Get_Bone(TEXT("Gun_r"));

	ParentMatrixDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	ParentMatrixDesc.OffsetMatrix = pBone->Get_OffsetMatrix();
	ParentMatrixDesc.pCombindTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
	ParentMatrixDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();

	if (FAILED(__super::Add_Part(LEVEL_BOSS, TEXT("Prototype_GameObject_Boss_Sword"), TEXT("Layer_EnemyWeapon"),
		TEXT("Part_Boss_Sword"), reinterpret_cast<CGameObject**>(&m_pSword), &ParentMatrixDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss::SetUp_BehaviorTree()
{
	CBlackBoard* pBlackBoard = CBlackBoard::Create();
	pBlackBoard->Add_Value(TEXT("Value_Owner"), this);
	pBlackBoard->Add_Value(TEXT("Value_Transform"), m_pTransformCom);
	pBlackBoard->Add_Value(TEXT("Value_Navigation"), m_pNavigationCom);
	pBlackBoard->Add_Value(TEXT("Value_Target"), &m_pTargetPlayer);
	pBlackBoard->Add_Value(TEXT("Value_isDead"), &m_isDead);
	/* Task Fly */
	m_fFlyHeight = 15.f;
	pBlackBoard->Add_Value(TEXT("Value_FlyHeight"), &m_fFlyHeight);
	m_fFlySpeed = 3.f;
	pBlackBoard->Add_Value(TEXT("Value_FlySpeed"), &m_fFlySpeed);
	/* RandomChoose_Move */
	pBlackBoard->Add_Value(TEXT("Value_isMoveFront"), &m_isMoveFront);
	pBlackBoard->Add_Value(TEXT("Value_isMoveBack"), &m_isMoveBack);
	pBlackBoard->Add_Value(TEXT("Value_isMoveRight"), &m_isMoveRight);
	pBlackBoard->Add_Value(TEXT("Value_isMoveLeft"), &m_isMoveLeft);
	m_dMoveTime = 2.0;
	pBlackBoard->Add_Value(TEXT("Value_MoveTime"), &m_dMoveTime);

	/* For. Com_BehaviorTree */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_BehaviorTree"),
		TEXT("Com_BehaviorTree"), reinterpret_cast<CComponent**>(&m_pBehaviorTreeCom),
		CSelector_Boss::Create(pBlackBoard))))
	{
		MSG_BOX("Failed CBoss Add_Component : (Com_BehaviorTree)");
		return E_FAIL;
	}

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
	m_eCurrentAnimationFlag = m_pModelCom->Get_AnimationState();

	// 애니메이션이 종료 되었을때 따로 처리되는 명령이 없을 경우 IDLE처리
	if (ANIM_FINISHED == m_eCurrentAnimationFlag &&
		m_ePreState == m_eCurState)
	{
		m_eCurState = STATE_IDLE;
	}

	if (STATE_DEAD != m_eCurState)
	{

	}

	Motion_Change(m_eCurrentAnimationFlag);

	m_pModelCom->Play_Animation(dTimeDelta);
}

void CBoss::Motion_Change(ANIMATIONFLAG eAnimationFlag)
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

		case STATE_DEAD:
			m_pModelCom->Set_AnimIndex(13 + rand() % 3, false);
			break;
		}

		m_ePreState = m_eCurState;
	}
}

void CBoss::Tick_Bomb(_double dTimeDelta)
{
	for (auto& pBomb : m_Bombs)
		pBomb->Tick(dTimeDelta);
}

void CBoss::Late_Tick_Bomb(_double dTimeDelta)
{
	for (auto iter = m_Bombs.begin(); iter != m_Bombs.end();)
	{
		GAMEEVENT eEvent = (*iter)->Late_Tick(dTimeDelta);
		if (GAME_OBJECT_DEAD == eEvent)
		{
			Safe_Release(*iter);
			iter = m_Bombs.erase(iter);
		}
		else
			++iter;
	}
}

GAMEEVENT CBoss::PlayEvent(_double dTimeDelta)
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

	for (auto& pBomb : m_Bombs)
		Safe_Release(pBomb);
	m_Bombs.clear();

	Safe_Release(m_pSword);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pVisionColliderCom);
	Safe_Release(m_pBehaviorTreeCom);
}
