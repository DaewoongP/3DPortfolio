#include "..\Public\Boss.h"
#include "GameInstance.h"
#include "Boss_Sword.h"
#include "Selector_Boss.h"
#include "BloodScreen.h"
#include "BloodParticle.h"
#include "BloodDirectional.h"
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
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(150.f, 0.f, 150.f, 1.f));

	CTransform::TRANSFORMDESC TransformDesc = CTransform::TRANSFORMDESC(3.f, XMConvertToRadians(90.f));
	m_pTransformCom->Set_Desc(TransformDesc);

	m_pModelCom->Delete_AnimationTranslation(1);
	m_pModelCom->Reset_Animation(14);
	m_pModelCom->Delete_AnimationTranslation(19);
	m_pModelCom->Delete_AnimationTranslation(21);

	m_dDeadTime = 4.0;
	m_iMaxHp = 8;
	m_iHp = m_iMaxHp;

#ifdef _DEBUG
	m_pVisionColliderCom->Set_Color(DirectX::Colors::Aquamarine);
	m_pKnockBackColliderCom->Set_Color(DirectX::Colors::HotPink);
#endif // _DEBUG

	return S_OK;
}

void CBoss::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
	m_pVisionColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
	m_pKnockBackColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	pGameInstance->Add_Collider(COLLISIONDESC::COLTYPE_ENEMY, m_pColliderCom);	
	pGameInstance->Add_Collider(COLLISIONDESC::COLTYPE_ENEMYVISION, m_pVisionColliderCom);

	if (0.7f > m_pModelCom->Get_CurrentNotifySpeed())
		m_pSword->Attack();

	Knockback(dTimeDelta);

	Safe_Release(pGameInstance);

	Tick_Bomb(dTimeDelta);
}

GAMEEVENT CBoss::Late_Tick(_double dTimeDelta)
{
	Late_Tick_Bomb(dTimeDelta);

	AnimationState(dTimeDelta);

	__super::Late_Tick(dTimeDelta);

	if (nullptr != m_pRendererCom)
	{
		// 보스는 따로 절두체 컬링 안함.
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
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
		if (STATE_ATTACK1_END == m_eCurState || STATE_ATTACK2_END == m_eCurState)
		{
			_float fCurrentFramePercent = m_pModelCom->Get_CurrentFramePercent();

			if (0.7f > fCurrentFramePercent)
			{
				m_iHp -= 1;

				m_pBloodParticle->Render_Effect(m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 2.f, 0.f, 0.f));
				m_pBloodEffect->Render_Effect(1.0, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 2.f, 0.f, 0.f));

				if (0 >= m_iHp)
				{
					CGameInstance* pGameInstance = CGameInstance::GetInstance();
					Safe_AddRef(pGameInstance);

					pGameInstance->SetUp_GameEvent(LEVEL_BOSS, TEXT("Layer_Enemy"), GAME_OBJECT_DEAD);

					Safe_Release(pGameInstance);

					m_eGameEvent = GAME_OBJECT_DEAD;

					if (wcswcs(CollisionDesc.pOtherOwner->Get_Tag(), TEXT("Katana")))
					{
						m_pBloodScreenEffect->Render_Effect(2.0);
					}
				}
			}
		}
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
		m_pShaderCom->Begin(2);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CBoss::Reset()
{
	// 무조건 위에 있어야함.
	if (FAILED(__super::Reset()))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	//pGameInstance->Clear_Layer(LEVEL_BOSS, TEXT("Layer_Enemy"));
	pGameInstance->Clear_DeadObjects(LEVEL_BOSS, TEXT("Layer_Enemy"));

	Safe_Release(pGameInstance);

	m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());
	m_pTransformCom->Set_Scale(_float3(1.f, 1.f, 1.f));
	m_pTransformCom->Rotation(_float3(0.f, 0.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(150.f, 0.f, 150.f, 1.f));

	CTransform::TRANSFORMDESC TransformDesc = CTransform::TRANSFORMDESC(6.f, XMConvertToRadians(90.f));
	m_pTransformCom->Set_Desc(TransformDesc);

	m_pModelCom->Reset_Animation(14);

	// 상태값 통일하면 애니메이션 초기화도 간단함.
	m_ePreState = STATE_IDLE;

	if (m_isFly)
		m_eCurState = STATE_FLY_IDLE;
	else
		m_eCurState = STATE_IDLE;

	/* BlackBoard Reset */
	m_iCurPatternCnt = 0;
	m_pTargetPlayer = nullptr;

	for (auto& pBomb : m_Bombs)
		pBomb->Reset();

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
		MSG_BOX("Failed CBoss Add_Component : (Com_Model)");
		return E_FAIL;
	}

	Add_Notifies();

	/* For.Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		MSG_BOX("Failed CBoss Add_Component : (Com_Shader)");
		return E_FAIL;
	}

	CBounding_AABB::BOUNDINGAABBDESC AABBDesc;

	AABBDesc.vExtents = _float3(1.f, 2.4f, 1.f);
	AABBDesc.vPosition = _float3(0.f, AABBDesc.vExtents.y, 0.f);

	/* For.Com_Collider */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &AABBDesc)))
	{
		MSG_BOX("Failed CBoss Add_Component : (Com_Collider)");
		return E_FAIL;
	}
	

	/* For.Com_KnockBackCollider */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_KnockBackCollider"), reinterpret_cast<CComponent**>(&m_pKnockBackColliderCom))))
	{
		MSG_BOX("Failed CBoss Add_Component : (Com_KnockBackCollider)");
		return E_FAIL;
	}

	CBounding_Sphere::BOUNDINGSPHEREDESC SphereDesc;

	SphereDesc.fRadius = 200.f;
	SphereDesc.vPosition = _float3(0.f, 0.f, 0.f);

	/* For.Com_VisionCollider */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_VisionCollider"), reinterpret_cast<CComponent**>(&m_pVisionColliderCom), &SphereDesc)))
	{
		MSG_BOX("Failed CBoss Add_Component : (Com_VisionCollider)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(CComposite::Add_Component(LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boss_Emissive"),
		TEXT("Com_EmissiveTexture"), reinterpret_cast<CComponent**>(&m_pEmissiveTextureCom))))
	{
		MSG_BOX("Failed CBoss Add_Component : (Com_EmissiveTexture)");
		return E_FAIL;
	}

	/* For.Com_BloodEffect */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_BloodDirectional"),
		TEXT("Com_BloodEffect"), reinterpret_cast<CComponent**>(&m_pBloodEffect))))
	{
		MSG_BOX("Failed CEnemy_Pistol Add_Component : (Com_BloodEffect)");
		return E_FAIL;
	}

	/* For.Com_BloodParticle */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_BloodParticle"),
		TEXT("Com_BloodParticle"), reinterpret_cast<CComponent**>(&m_pBloodParticle))))
	{
		MSG_BOX("Failed CEnemy_Pistol Add_Component : (Com_BloodParticle)");
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

	if (FAILED(__super::Add_Part(LEVEL_BOSS, TEXT("Prototype_GameObject_Boss_Sword"), TEXT("Layer_BossWeapon"),
		TEXT("Part_Boss_Weapon"), reinterpret_cast<CGameObject**>(&m_pSword), &ParentMatrixDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss::Add_Notifies()
{
	if (FAILED(SetUp_AnimationNotifies(TEXT("../../Resources/GameData/Notify/Sequence1.Notify"))))
		return E_FAIL;
	if (FAILED(SetUp_AnimationNotifies(TEXT("../../Resources/GameData/Notify/Sequence2.Notify"))))
		return E_FAIL;
	if (FAILED(SetUp_AnimationNotifies(TEXT("../../Resources/GameData/Notify/Sequence1_End.Notify"))))
		return E_FAIL;
	if (FAILED(SetUp_AnimationNotifies(TEXT("../../Resources/GameData/Notify/Sequence2_End.Notify"))))
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
	pBlackBoard->Add_Value(TEXT("Value_CurPatternCnt"), &m_iCurPatternCnt);
	m_iMaxPatternCnt = 2;
	pBlackBoard->Add_Value(TEXT("Value_MaxPatternCnt"), &m_iMaxPatternCnt);
	/* Task Fly */
	m_fFlyHeight = 15.f;
	pBlackBoard->Add_Value(TEXT("Value_FlyHeight"), &m_fFlyHeight);
	m_fFlySpeed = 3.f;
	pBlackBoard->Add_Value(TEXT("Value_FlySpeed"), &m_fFlySpeed);
	pBlackBoard->Add_Value(TEXT("Value_isFly"), &m_isFly);
	/* RandomChoose_Move */
	pBlackBoard->Add_Value(TEXT("Value_isMoveFront"), &m_isMoveFront);
	pBlackBoard->Add_Value(TEXT("Value_isMoveBack"), &m_isMoveBack);
	pBlackBoard->Add_Value(TEXT("Value_isMoveRight"), &m_isMoveRight);
	pBlackBoard->Add_Value(TEXT("Value_isMoveLeft"), &m_isMoveLeft);
	m_dMoveTime = 2.0;
	pBlackBoard->Add_Value(TEXT("Value_MoveTime"), &m_dMoveTime);
	/* Task Landing */
	m_dLandingSpeed = 6.0;
	pBlackBoard->Add_Value(TEXT("Value_LandingSpeed"), &m_dLandingSpeed);
	/* Task Charge */
	m_dChargeReadyTime = 1.0;
	pBlackBoard->Add_Value(TEXT("Value_ChargeReadyTime"), &m_dChargeReadyTime);
	m_dChargeSpeed = 15.0;
	pBlackBoard->Add_Value(TEXT("Value_ChargeSpeed"), &m_dChargeSpeed);
	pBlackBoard->Add_Value(TEXT("Value_isCharge"), &m_isCharge);
	/* Task AttackSequence */
	m_dAttack1Time = 4.0;
	pBlackBoard->Add_Value(TEXT("Value_AttackSequence1Time"), &m_dAttack1Time);
	pBlackBoard->Add_Value(TEXT("Value_isAttackSequence1"), &m_isAttack1);
	m_dAttack2Time = 7.0;
	pBlackBoard->Add_Value(TEXT("Value_AttackSequence2Time"), &m_dAttack2Time);
	pBlackBoard->Add_Value(TEXT("Value_isAttackSequence2"), &m_isAttack2);

	pBlackBoard->Add_Value(TEXT("Value_isAttackFinished"), &m_isAttackFinished);

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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPos", pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;
	_float fRimWidth = 1.f;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fRimWidth", &fRimWidth, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pEmissiveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_EmissiveTexture")))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CBoss::SetUp_AnimationNotifies(const _tchar* pNotifyFilePath)
{
	_uint				iAnimationIndex = { 0 };
	_uint				iAnimationFrames = { 0 };
	vector<NOTIFY>		Notifies;

	HANDLE hFile = CreateFile(pNotifyFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong	dwByte = 0;

	// Current Animation Index
	ReadFile(hFile, &iAnimationIndex, sizeof(_uint), &dwByte, nullptr);
	// Animation's NumFrames
	ReadFile(hFile, &iAnimationFrames, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iAnimationFrames; ++i)
	{
		NOTIFY Notify;
		// Notify Save
		ReadFile(hFile, &Notify, sizeof(NOTIFY), &dwByte, nullptr);

		Notifies.push_back(Notify);
	}

	CloseHandle(hFile);

	// for문 돌면서 read file. 하면서 값다 처리.
	if (FAILED(m_pModelCom->SetUp_AnimationNotifies(iAnimationIndex, Notifies)))
		return E_FAIL;

	return S_OK;
}

void CBoss::AnimationState(_double dTimeDelta)
{
	m_eCurrentAnimationFlag = m_pModelCom->Get_AnimationState();

	// 애니메이션이 종료 되었을때 따로 처리되는 명령이 없을 경우 IDLE처리
	if (ANIM_FINISHED == m_eCurrentAnimationFlag &&
		m_ePreState == m_eCurState)
	{
		if (true == m_isCharge &&
			m_eCurState != STATE_IDLETOCHARGE)
			m_eCurState = STATE_CHARGE;
		else if (STATE_ATTACKTOFLY == m_eCurState)
		{
			m_isAttackFinished = false;
			m_isFly = true;
			m_iCurPatternCnt = 0;
			m_eCurState = STATE_FLY_IDLE;
		}
		else if (STATE_ATTACK1_END == m_eCurState ||
			STATE_ATTACK2_END == m_eCurState)
			m_eCurState = STATE_ATTACKTOFLY;
		else
		{
			m_eCurState = STATE_IDLE;
		}
	}

	if (STATE_DEAD != m_eCurState &&
		STATE_ATTACKTOFLY != m_eCurState)
	{
		if (true == m_isFly)
		{
			m_eCurState = STATE_FLY_IDLE;

			if (true == m_isMoveFront)
				m_eCurState = STATE_FLY_FRONT;
			else if (true == m_isMoveBack)
				m_eCurState = STATE_FLY_BACK;
			else if (true == m_isMoveLeft)
				m_eCurState = STATE_FLY_LEFT;
			else if (true == m_isMoveRight)
				m_eCurState = STATE_FLY_RIGHT;
		}
		else
		{
			m_eCurState = STATE_IDLE;

			if (true == m_isCharge &&
				m_eCurState != STATE_CHARGE)
				m_eCurState = STATE_IDLETOCHARGE;

			if (true == m_isAttack1)
			{
				m_eCurState = STATE_ATTACK1;
				m_ePreAttack = STATE_ATTACK1;
			}
			if (true == m_isAttack2)
			{
				m_eCurState = STATE_ATTACK2;
				m_ePreAttack = STATE_ATTACK2;
			}
			
			if (true == m_isAttackFinished &&
				STATE_ATTACK1 == m_ePreAttack)
				m_eCurState = STATE_ATTACK1_END;
			else if (true == m_isAttackFinished &&
				STATE_ATTACK2 == m_ePreAttack)
				m_eCurState = STATE_ATTACK2_END;
		}
	}

	Motion_Change(m_eCurrentAnimationFlag);

	if (STATE_DEAD == m_eCurState)
		m_pModelCom->Play_Animation(dTimeDelta, false, 0.5);
	else
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
			m_pModelCom->Set_AnimIndex(14);
			break;
		case STATE_FLY_IDLE:
			m_pModelCom->Set_AnimIndex(11);
			break;
		case STATE_FLY_FRONT:
			m_pModelCom->Set_AnimIndex(5);
			break;
		case STATE_FLY_BACK:
			m_pModelCom->Set_AnimIndex(3);
			break;
		case STATE_FLY_LEFT:
			m_pModelCom->Set_AnimIndex(10);
			break;
		case STATE_FLY_RIGHT:
			m_pModelCom->Set_AnimIndex(12);
			break;
		case STATE_FLY_HIT:
			m_pModelCom->Set_AnimIndex(7 + rand() % 2, false);
			break;
		case STATE_IDLETOCHARGE:
			m_pModelCom->Set_AnimIndex(19, false);
			break;
		case STATE_CHARGE:
			m_pModelCom->Set_AnimIndex(1);
			break;
		case STATE_ATTACK1:
			m_pModelCom->Set_AnimIndex(15, false);
			break;
		case STATE_ATTACK1_END:
			m_pModelCom->Set_AnimIndex(16, false);
			break;
		case STATE_ATTACK2:
			m_pModelCom->Set_AnimIndex(17, false);
			break;
		case STATE_ATTACK2_END:
			m_pModelCom->Set_AnimIndex(18, false);
			break;
		case STATE_ATTACKTOFLY:
			m_pModelCom->Set_AnimIndex(2, false);
			break;
		case STATE_DEAD:
			m_pModelCom->Set_AnimIndex(21, false);
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

void CBoss::Knockback(_double dTimeDelta)
{
	if (false == (STATE_ATTACK1_END == m_eCurState || STATE_ATTACK2_END == m_eCurState))
		return;

	_float fCurrentFramePercent = m_pModelCom->Get_CurrentFramePercent();

	if (0.6f < fCurrentFramePercent)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		CBounding_AABB::BOUNDINGAABBDESC AABBDesc;

		AABBDesc.vExtents = _float3(10.f, 10.f, 10.f);
		_vector vColPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_State(CTransform::STATE_LOOK) * 10.f;
		XMStoreFloat3(&AABBDesc.vPosition, vColPos);
		m_pKnockBackColliderCom->Set_BoundingDesc(&AABBDesc);
		pGameInstance->Add_Collider(COLLISIONDESC::COLTYPE_KNOCKBACK, m_pKnockBackColliderCom);

		Safe_Release(pGameInstance);
	}
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

		m_dDeadAnimAcc += dTimeDelta;

		if (m_dDeadTime < m_dDeadAnimAcc)
			return GAME_OBJECT_DEAD;
		//if (ANIM_FINISHED == m_eCurrentAnimationFlag)
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

	Safe_Release(m_pBloodEffect);
	Safe_Release(m_pBloodParticle);

	Safe_Release(m_pSword);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pVisionColliderCom);
	Safe_Release(m_pEmissiveTextureCom);
	Safe_Release(m_pKnockBackColliderCom);
	Safe_Release(m_pBehaviorTreeCom);
}
