#include "..\Public\Enemy_Pistol.h"
#include "GameInstance.h"
#include "Selector_FindTargetToAttack.h"
#include "BloodDirectional.h"
#include "BloodScreen.h"
#include "Pistol.h"

CEnemy_Pistol::CEnemy_Pistol(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEnemy(pDevice, pContext)
{
}

CEnemy_Pistol::CEnemy_Pistol(const CEnemy_Pistol& rhs)
	: CEnemy(rhs)
{
}

HRESULT CEnemy_Pistol::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnemy_Pistol::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		m_EnemyDesc = *(static_cast<ENEMYDESC*>(pArg));
		lstrcpy(m_EnemyDesc.pModelPrototypeTag, (static_cast<ENEMYDESC*>(pArg))->pModelPrototypeTag);
	}
	else
	{
		MSG_BOX("Failed Read EnemyDesc");
		return E_FAIL;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(m_EnemyDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnemy_Pistol::Initialize_Level(_uint iLevelIndex)
{
	if (FAILED(Add_Parts(iLevelIndex)))
		return E_FAIL;

	m_pTransformCom->Set_Scale(m_EnemyDesc.vScale);
	m_pTransformCom->Rotation(m_EnemyDesc.vRotation);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_EnemyDesc.vPosition));

	CTransform::TRANSFORMDESC TransformDesc = CTransform::TRANSFORMDESC(3.f, XMConvertToRadians(360.f));
	m_pTransformCom->Set_Desc(TransformDesc);

	m_pModelCom->Reset_Animation(54);

#ifdef _DEBUG
	m_pVisionColliderCom->Set_Color(DirectX::Colors::Aquamarine);
#endif // _DEBUG

	if (FAILED(__super::Initialize_Level(iLevelIndex)))
		return E_FAIL;

	if (FAILED(SetUp_BehaviorTree()))
		return E_FAIL;

	return S_OK;
}

void CEnemy_Pistol::Tick(_double dTimeDelta)
{
	// 셀 Y값과 차이나면 땅으로 떨굼
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (m_pNavigationCom->Get_CurrentCellY(vPos) < XMVectorGetY(vPos))
	{
		m_pTransformCom->Move_Direction(XMVectorSet(0.f, -1.f, 0.f, 0.f), dTimeDelta * 20.0);

		_float fY = XMVectorGetY(vPos) - m_pNavigationCom->Get_CurrentCellY(vPos);
		if (1.f > fY)
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pNavigationCom->Get_CurrentCellY(vPos)));
	}

	__super::Tick(dTimeDelta);

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
	m_pVisionColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	pGameInstance->Add_Collider(COLLISIONDESC::COLTYPE_ENEMY, m_pColliderCom);
	pGameInstance->Add_Collider(COLLISIONDESC::COLTYPE_ENEMYVISION, m_pVisionColliderCom);

	Safe_Release(pGameInstance);
}

GAMEEVENT CEnemy_Pistol::Late_Tick(_double dTimeDelta)
{
	AnimationState(dTimeDelta);

	__super::Late_Tick(dTimeDelta);

	const CBone* pBone = m_pModelCom->Get_Bone(TEXT("head_end"));
	XMStoreFloat4x4(&m_LensOffsetMatrix, 
		XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixRotationY(XMConvertToRadians(90.f)) *
		XMLoadFloat4x4(pBone->Get_CombinedTransformationMatrixPtr()) * m_pModelCom->Get_PivotMatrix() 
		* XMMatrixTranslation(0.f, -0.2f, 0.35f));

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (nullptr != m_pRendererCom &&
		true == pGameInstance->isIn_WorldFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 3.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_LIGHTDEPTH, this);

#ifdef _DEBUG
		m_pRendererCom->Add_DebugGroup(m_pNavigationCom);
		m_pRendererCom->Add_DebugGroup(m_pColliderCom);
		m_pRendererCom->Add_DebugGroup(m_pVisionColliderCom);
#endif // _DEBUG
	}

	Safe_Release(pGameInstance);

	return PlayEvent(dTimeDelta);
}

void CEnemy_Pistol::OnCollisionEnter(COLLISIONDESC CollisionDesc)
{
	if (CollisionDesc.pMyCollider == m_pColliderCom &&
		COLLISIONDESC::COLTYPE_PLAYERWEAPON == CollisionDesc.ColType)
	{
		m_eGameEvent = GAME_OBJECT_DEAD;

		if (wcswcs(CollisionDesc.pOtherOwner->Get_Tag(), TEXT("Katana")))
		{
			m_pBloodScreenEffect->Render_Effect(2.0);
		}
	}	
}

void CEnemy_Pistol::OnCollisionStay(COLLISIONDESC CollisionDesc)
{
	if (CollisionDesc.pMyCollider == m_pVisionColliderCom &&
		COLLISIONDESC::COLTYPE_PLAYER == CollisionDesc.ColType)
		m_pTargetPlayer = CollisionDesc.pOtherOwner;

	__super::OnCollisionStay(CollisionDesc);
}

void CEnemy_Pistol::OnCollisionExit(COLLISIONDESC CollisionDesc)
{
	if (CollisionDesc.pMyCollider == m_pVisionColliderCom &&
		COLLISIONDESC::COLTYPE_PLAYER == CollisionDesc.ColType)
		m_pTargetPlayer = nullptr;
}

HRESULT CEnemy_Pistol::Render()
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

		m_pShaderCom->Begin(2);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CEnemy_Pistol::Render_LightDepth()
{
	if (FAILED(SetUp_ShadowShaderResources()))
		return E_FAIL;

	// 그림자 매핑 다른 렌더링은 똑같이 처리하면됨.
	// 근데 일단 노말텍스처나 이런 머테리얼 바인딩은 필요없어보임.
	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShadowShaderCom, "g_BoneMatrices", i);

		m_pShadowShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CEnemy_Pistol::Reset()
{
	m_pModelCom->Reset_Animation(54);
	m_ePreState = STATE_END;
	m_eCurState = STATE_IDLE;

	m_pTargetPlayer = nullptr;
	m_isWait = false;
	m_isAttack = false;
	m_isWalk = false;
	m_bBloodEffect = false;

	if (FAILED(__super::Reset()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnemy_Pistol::Add_Component(ENEMYDESC& EnemyDesc)
{
	/* For.Com_Model */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Enemy_Pistol"),
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

	/* For.Com_ShadowShader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimMesh_Shadow"),
		TEXT("Com_ShadowShader"), reinterpret_cast<CComponent**>(&m_pShadowShaderCom))))
	{
		MSG_BOX("Failed CEnemy_Pistol Add_Component : (Com_ShadowShader)");
		return E_FAIL;
	}

	CBounding_AABB::BOUNDINGAABBDESC AABBDesc;

	AABBDesc.vExtents = _float3(1.f, 2.4f, 1.f);
	AABBDesc.vPosition = _float3(0.f, AABBDesc.vExtents.y, 0.f);

	/* For.Com_Collider */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &AABBDesc)))
	{
		MSG_BOX("Failed CEnemy_Pistol Add_Component : (Com_Collider)");
		return E_FAIL;
	}

	CBounding_Sphere::BOUNDINGSPHEREDESC SphereDesc;

	SphereDesc.fRadius = 20.f;
	SphereDesc.vPosition = _float3(0.f, 0.f, 0.f);
	
	/* For.Com_VisionCollider */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_VisionCollider"), reinterpret_cast<CComponent**>(&m_pVisionColliderCom), &SphereDesc)))
	{
		MSG_BOX("Failed CEnemy_Pistol Add_Component : (Com_VisionCollider)");
		return E_FAIL;
	}
	
	/* For.Com_BloodEffect */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_BloodDirectional"),
		TEXT("Com_BloodEffect"), reinterpret_cast<CComponent**>(&m_pBloodEffect))))
	{
		MSG_BOX("Failed CEnemy_Pistol Add_Component : (Com_BloodEffect)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEnemy_Pistol::Add_Parts(_uint iLevelIndex)
{
	CPart::PARENTMATRIXDESC ParentMatrixDesc;
	ZEROMEM(&ParentMatrixDesc);

	const CBone* pBone = m_pModelCom->Get_Bone(TEXT("Gun_r"));

	ParentMatrixDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	ParentMatrixDesc.OffsetMatrix = pBone->Get_OffsetMatrix();
	ParentMatrixDesc.pCombindTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
	ParentMatrixDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();

	if (FAILED(__super::Add_Part(iLevelIndex, TEXT("Prototype_GameObject_Weapon_Pistol"), TEXT("Layer_EnemyPart"),
		TEXT("Part_Pistol"), reinterpret_cast<CGameObject**>(&m_pPistol), &ParentMatrixDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnemy_Pistol::SetUp_BehaviorTree()
{
	CBlackBoard* pBlackBoard = CBlackBoard::Create();
	pBlackBoard->Add_Value(TEXT("Value_Transform"), m_pTransformCom);
	pBlackBoard->Add_Value(TEXT("Value_Navigation"), m_pNavigationCom);
	
	pBlackBoard->Add_Value(TEXT("Value_LensFlare"), m_pLensFlareEffect);
	pBlackBoard->Add_Value(TEXT("Value_LensMatrix"), &m_LensOffsetMatrix);

	pBlackBoard->Add_Value(TEXT("Value_Target"), &m_pTargetPlayer);
	pBlackBoard->Add_Value(TEXT("Value_isWalk"), &m_isWalk);
	pBlackBoard->Add_Value(TEXT("Value_isDead"), &m_isDead);

	m_dPatrolWaitTime = 8.0;
	pBlackBoard->Add_Value(TEXT("Value_MaxWaitTime"), &m_dPatrolWaitTime);
	pBlackBoard->Add_Value(TEXT("Value_isWait"), &m_isWait);

	m_dAttackCoolTime = 1.0;
	pBlackBoard->Add_Value(TEXT("Value_AttackCoolTime"), &m_dAttackCoolTime);
	pBlackBoard->Add_Value(TEXT("Value_isAttack"), &m_isAttack);
	pBlackBoard->Add_Value(TEXT("Value_Weapon"), m_pPistol);

	/* For. Com_BehaviorTree */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_BehaviorTree"),
		TEXT("Com_BehaviorTree"), reinterpret_cast<CComponent**>(&m_pBehaviorTreeCom),
		CSelector_FindTargetToAttack::Create(pBlackBoard))))
	{
		MSG_BOX("Failed CEnemy_Pistol Add_Component : (Com_BehaviorTree)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEnemy_Pistol::SetUp_ShaderResources()
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

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CEnemy_Pistol::SetUp_ShadowShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShadowShaderCom->Bind_RawValue("g_fFar", pGameInstance->Get_CamFar(), sizeof(_float))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	if (FAILED(m_pShadowShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	// 빛이 바라보는 기준으로 그릴것이므로
	// 빛기준으로 돌린 행렬을 던진다.
	_float4x4 LightViewMatrix;
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	XMStoreFloat4x4(&LightViewMatrix, XMMatrixLookAtLH(vPos + XMVectorSet(-20.f, 20.f, -20.f, 1.f), vPos, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	if (FAILED(m_pShadowShaderCom->Bind_Matrix("g_ViewMatrix", &LightViewMatrix)))
		return E_FAIL;
	_float4x4 LightProjMatrix;
	XMStoreFloat4x4(&LightProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(120.f), (_float)g_iWinSizeX / g_iWinSizeY, 0.1f, 1000.f));
	if (FAILED(m_pShadowShaderCom->Bind_Matrix("g_ProjMatrix", &LightProjMatrix)))
		return E_FAIL;
	
	return S_OK;
}

void CEnemy_Pistol::AnimationState(_double dTimeDelta)
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
			m_eCurState = STATE_ATTACK;
	}

	Motion_Change(m_eCurrentAnimationFlag);
	
	m_pModelCom->Play_Animation(dTimeDelta);
}

void CEnemy_Pistol::Motion_Change(ANIMATIONFLAG eAnimationFlag)
{
	// 모션 변경 처리 전 예외처리.
	if (false == (ANIM_PLAYING == eAnimationFlag || ANIM_FINISHED == eAnimationFlag))
		return;

	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case STATE_IDLE:
			m_pModelCom->Set_AnimIndex(54);
			break;
		case STATE_ATTACK:
			m_pModelCom->Set_AnimIndex(97, false);
			break;
		case STATE_WALK:
			m_pModelCom->Set_AnimIndex(62);
			break;
		case STATE_DEAD:
			m_pModelCom->Set_AnimIndex(51 + rand() % 3, false);
			break;
		}

		m_ePreState = m_eCurState;
	}
}

GAMEEVENT CEnemy_Pistol::PlayEvent(_double dTimeDelta)
{
	if (GAME_OBJECT_DEAD == m_eGameEvent)
	{
		if (false == m_bBloodEffect)
		{
			m_pBloodEffect->Render_Effect(0.2, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 2.f, 0.f, 1.f));
			m_bBloodEffect = true;
		}
		
		m_eCurState = STATE_DEAD;
		m_isDead = true;

		if (ANIM_FINISHED == m_eCurrentAnimationFlag)
			return GAME_OBJECT_DEAD;
	}

	return GAME_NOEVENT;
}

CEnemy_Pistol* CEnemy_Pistol::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEnemy_Pistol* pInstance = new CEnemy_Pistol(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CEnemy_Pistol");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEnemy_Pistol::Clone(void* pArg)
{
	CEnemy_Pistol* pInstance = new CEnemy_Pistol(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CEnemy_Pistol");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEnemy_Pistol::Free()
{
	__super::Free();

	Safe_Release(m_pBloodEffect);

	Safe_Release(m_pPistol);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShadowShaderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pVisionColliderCom);
	Safe_Release(m_pBehaviorTreeCom);
}
