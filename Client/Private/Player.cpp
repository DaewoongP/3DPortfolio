#include "..\Public\Player.h"
#include "GameInstance.h"
#include "Part.h"
#include "Layer.h"
#include "Katana.h"
#include "Shuriken.h"
#include "Sword.h"
#include "Wire.h"
#include "Surge.h"
#include "Bullet.h"
#include "ColProp.h"
#include "BlockEffect.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CGameObject(rhs)

#ifdef _DEBUG
	, m_isMouseFixed(rhs.m_isMouseFixed)
#endif // _DEBUG
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	m_fSpeed = 4.f;
	m_fJumpPower = 20.f;
	m_fWallRunVelocity = 0.6f;
	m_fHookPower = 2.2f;
	m_dBlinkTime = 3.0;
	m_fAttackCamMove = 7.f;

	CTransform::TRANSFORMDESC TransformDesc = CTransform::TRANSFORMDESC(m_fSpeed, XMConvertToRadians(3.f));

	if (FAILED(__super::Initialize(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	if (FAILED(Add_Notifies()))
		return E_FAIL;

	if (FAILED(Initailize_Skills()))
		return E_FAIL;

	m_eCurrentSkill = SKILL_BLINK;

	return S_OK;
}

HRESULT CPlayer::Initialize_Level(_uint iLevelIndex)
{
	XMStoreFloat3(&m_vInitRotation, XMVectorSet(0.f, 90.f, 0.f, 0.f));

	switch ((LEVELID)iLevelIndex)
	{
	case LEVEL_STAGE1:
		XMStoreFloat4(&m_vInitPosition, XMVectorSet(40.f, 0.f, 90.f, 1.f));
		break;
	case LEVEL_BOSS:
		XMStoreFloat4(&m_vInitPosition, XMVectorSet(100.f, 0.f, 150.f, 1.f));
		break;
	default:
		XMStoreFloat4(&m_vInitPosition, XMVectorSet(0.f, 0.f, 0.f, 1.f));
		break;
	}
	
	if (FAILED(Add_Component_Level(iLevelIndex)))
		return E_FAIL;

	if (FAILED(Add_Parts(iLevelIndex)))
		return E_FAIL;

	m_pTransformCom->Rotation(m_vInitRotation);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_vInitPosition));
	// 카메라 초기 값을 객체의 트랜스폼 월드값으로 초기화.
	m_pPlayerCameraCom->Set_CameraWorldMatrix(XMLoadFloat4x4(m_pTransformCom->Get_WorldFloat4x4()));
	// 모델의 애니메이션 인덱스 설정
	m_pModelCom->Set_AnimIndex(95);
	m_eCurState = STATE_IDLE;

	m_fMouseSensitivity = 0.2f;

	m_pTransformCom->Use_RigidBody(m_pNavigationCom);

	XMStoreFloat4x4(&m_CameraMatrix, XMMatrixIdentity());

	m_pNavigationCom->Find_MyCell(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_eCurWeapon = WEAPON_KATANA;

	m_iSkillMaxStack = 4;

#ifdef _DEBUG
	m_pVisionColliderCom->Set_Color(DirectX::Colors::AntiqueWhite);
	m_pBlockColliderCom->Set_Color(DirectX::Colors::DarkRed);
	m_pBlinkColliderCom->Set_Color(DirectX::Colors::Yellow);
#endif // _DEBUG

	return S_OK;
}

void CPlayer::Tick(_double dTimeDelta)
{
	Key_Input(dTimeDelta);
	Fix_Mouse();

	SwapWeapon();

	AnimationState(dTimeDelta);

#ifdef _DEBUG
	if (m_isInvisible)
	{
		m_eGameEvent = GAME_NOEVENT;
		if (STATE_DEAD == m_eCurState)
			m_eCurState = m_ePreState;
	}
#endif // _DEBUG
	
	if (GAME_OBJECT_DEAD == m_eGameEvent)
	{
		m_pRendererCom->Set_GrayScale(true);
		dTimeDelta *= 0.1f;
		m_pTransformCom->ZeroVelocity();
	}

	m_pTransformCom->Crouch(m_isCrouch, dTimeDelta, 4.f, 2.f, 1.5f);
	
	Attack(dTimeDelta);
	Block(dTimeDelta);
	
	Tick_Skills(dTimeDelta);

	WallRunCameraReset(dTimeDelta);

	__super::Tick(dTimeDelta);

	// 카메라 포지션 고정, 카메라 회전처리 이후 포지션 변경
	//m_pModelCom->Invalidate_AnimationCamera(m_pPlayerCameraCom, m_pTransformCom, dTimeDelta);
	CameraOffset(dTimeDelta);

	CameraMove(dTimeDelta);
	// 객체포지션 -> 카메라 오프셋 고정 -> 카메라 상태행렬 갱신 -> 이후 처리 순서를 맞추기위해 함수이름 바꿈.
	m_pPlayerCameraCom->Tick_Camera(dTimeDelta);

	Check_Fall();

	Add_Collisions();
}

GAMEEVENT CPlayer::Late_Tick(_double dTimeDelta)
{
	SoundState(dTimeDelta);

#ifdef _DEBUG
	if (m_isInvisible)
		m_eGameEvent = GAME_NOEVENT;
#endif // _DEBUG

	if (GAME_STAGE_RESET != m_eGameEvent &&
		nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
#ifdef _DEBUG
		m_pRendererCom->Add_DebugGroup(m_pColliderCom);
		m_pRendererCom->Add_DebugGroup(m_pVisionColliderCom);
		m_pRendererCom->Add_DebugGroup(m_pBlockColliderCom);
		m_pRendererCom->Add_DebugGroup(m_pBlinkColliderCom);
		m_pRendererCom->Add_DebugGroup(m_pNavigationCom);
#endif // _DEBUG
	}

	__super::Late_Tick(dTimeDelta);

	if (GAME_OBJECT_DEAD == m_eGameEvent)
		return GAME_NOEVENT;

	if (true == m_pTransformCom->IsJumping() &&
		STATE_RUN == m_eCurState)
		m_eCurState = STATE_IDLE;

	return m_eGameEvent;
}

void CPlayer::OnCollisionEnter(COLLISIONDESC CollisionDesc)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	/* Player Collider */
	// Wall Run
	if (CollisionDesc.pMyCollider == m_pColliderCom &&
		!lstrcmp(CollisionDesc.pOtherOwner->Get_LayerTag(), TEXT("Layer_ColProps")))
	{
		m_fWallRunY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		
		if (m_fWallRunY > m_pTransformCom->Get_CurrentGroundY() + 0.5f)
		{
			m_fWallRunAngle = 0.f;
			m_isWallRun = { true };
		}
		else
		{
			m_fWallRunY = 0.f;
		}
	}

	if (CollisionDesc.pMyCollider == m_pColliderCom &&
		!lstrcmp(CollisionDesc.pOtherOwner->Get_LayerTag(), TEXT("Layer_EnemyWeapon")))
	{
		pGameInstance->Play_Sound(TEXT("Player_Death.ogg"), CSound_Manager::SOUND_PLAYER, 0.3f, true);
		m_eGameEvent = GAME_OBJECT_DEAD;
	}

	if (CollisionDesc.pMyCollider == m_pColliderCom &&
		COLLISIONDESC::COLTYPE_KNOCKBACK == CollisionDesc.ColType)
	{
		m_pTransformCom->Jump(CollisionDesc.pOtherTransform->Get_State(CTransform::STATE_LOOK) + XMVectorSet(0.f, 3.f, 0.f, 0.f), 40.f, g_TimeDelta);
	}

	/* Player Vision Collider */
	if (WEAPON_KATANA == m_eCurWeapon &&
		CollisionDesc.pMyCollider == m_pVisionColliderCom &&
		!lstrcmp(CollisionDesc.pOtherOwner->Get_LayerTag(), TEXT("Layer_Enemy")))
	{
		// 이동공격용 벡터
		m_InRangeEnemyColliders.push_back(CollisionDesc.pOtherCollider);
	}
	
	/* Player Block Collider */
	if (CollisionDesc.pMyCollider == m_pBlockColliderCom &&
		!lstrcmp(CollisionDesc.pOtherOwner->Get_LayerTag(), TEXT("Layer_EnemyWeapon")))
	{
		m_BlockEnemyWeapons.push_back(CollisionDesc.pOtherOwner);
	}

	if (CollisionDesc.pMyCollider == m_pBlockColliderCom &&
		COLLISIONDESC::COLTYPE_PARRYING == CollisionDesc.ColType)
	{
		// 보스 패링 패턴.
		m_BlockEnemyWeapons.push_back(CollisionDesc.pOtherOwner);
	}

	/* Player Blink Collider */
	if (m_pBlinkColliderCom == CollisionDesc.pMyCollider &&
		!lstrcmp(CollisionDesc.pOtherOwner->Get_LayerTag(), TEXT("Layer_Enemy")))
	{
		m_BlinkEnemys.push_back(CollisionDesc.pOtherOwner);
	}
	
	Safe_Release(pGameInstance);
}

void CPlayer::OnCollisionStay(COLLISIONDESC CollisionDesc)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (!lstrcmp(CollisionDesc.pOtherOwner->Get_LayerTag(), TEXT("Layer_ColProps")))
	{
		if (m_fWallRunY > 4.f)
		{
			CollisionStayWall(CollisionDesc);
		}
	}

	Safe_Release(pGameInstance);
}

void CPlayer::OnCollisionExit(COLLISIONDESC CollisionDesc)
{
	/* Player Collider */

	if (m_fWallRunY > 4.f &&
		!lstrcmp(CollisionDesc.pOtherOwner->Get_LayerTag(), TEXT("Layer_ColProps")))
	{
		m_isWallRun = { false };
		m_fWallRunY = 0.f;
	}

	/* Player Vision Collider */

	if (CollisionDesc.pMyCollider == m_pVisionColliderCom &&
		!lstrcmp(CollisionDesc.pOtherOwner->Get_LayerTag(), TEXT("Layer_Enemy")))
	{
		for (auto iter = m_InRangeEnemyColliders.begin(); iter != m_InRangeEnemyColliders.end(); ++iter)
		{
			if (*iter == CollisionDesc.pOtherCollider)
			{
				m_InRangeEnemyColliders.erase(iter);
				break;
			}
		}
	}

	if (CollisionDesc.pMyCollider == m_pBlockColliderCom &&
		!lstrcmp(CollisionDesc.pOtherOwner->Get_LayerTag(), TEXT("Layer_EnemyWeapon")))
	{
		for (auto iter = m_BlockEnemyWeapons.begin(); iter != m_BlockEnemyWeapons.end(); ++iter)
		{
			if (*iter == CollisionDesc.pOtherOwner)
			{
				m_BlockEnemyWeapons.erase(iter);
				break;
			}
		}
	}

	if (CollisionDesc.pMyCollider == m_pBlockColliderCom &&
		COLLISIONDESC::COLTYPE_PARRYING == CollisionDesc.ColType)
	{
		// 보스 패링 패턴.
		if (0 < m_BlockEnemyWeapons.size())
			m_eGameEvent = GAME_OBJECT_DEAD;
	}
}

HRESULT CPlayer::Render()
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

HRESULT CPlayer::Reset()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CCamera::CAMERADESC CameraDesc;
	CameraDesc.fFovy = XMConvertToRadians(90.f);
	CameraDesc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 1000.f;
	CameraDesc.TransformDesc.dSpeedPerSec = m_pTransformCom->Get_Desc().dSpeedPerSec;
	CameraDesc.TransformDesc.dRotationPerSec = m_pTransformCom->Get_Desc().dRotationPerSec;
	m_pPlayerCameraCom->Set_CameraDesc(CameraDesc);

	m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());
	m_pTransformCom->Rotation(m_vInitRotation);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_vInitPosition));
	// 카메라 초기 값을 객체의 트랜스폼 월드값으로 초기화.
	m_pPlayerCameraCom->Set_CameraWorldMatrix(XMLoadFloat4x4(m_pTransformCom->Get_WorldFloat4x4()));
	// 모델의 애니메이션 인덱스 설정
	m_pModelCom->Set_AnimIndex(95);

	m_pRendererCom->Set_GrayScale(false);
	m_pRendererCom->Set_RedScale(false);

	m_eCurState = STATE_IDLE;
	m_iSkillStack = 0;
	pGameInstance->Set_SlowedTime(TEXT("MainTimer"), 1.0);
	m_dBlinkTimeAcc = 0.0;

	m_InRangeEnemyColliders.clear();
	m_BlockEnemyWeapons.clear();
	m_BlinkEnemys.clear();

	// 표창 -> 검
	if (WEAPON_SHURIKEN == m_eCurWeapon)
	{
		if (FAILED(__super::Delete_Component(TEXT("Part_Shuriken"))))
			return E_FAIL;

		m_Components.emplace(TEXT("Part_Katana"), m_pKatana);
		Safe_AddRef(m_pKatana);

		m_eCurWeapon = WEAPON_KATANA;
	}

	m_bSwapWeapon = false;
	
	Safe_Release(pGameInstance);

	// 컴포넌트 리셋들 모두 호출해주려면 부모 불러줘야함.
	if (FAILED(__super::Reset()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Add_Component()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Player"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		MSG_BOX("Failed CPlayer Add_Component : (Com_Model)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		MSG_BOX("Failed CPlayer Add_Component : (Com_Shader)");
		return E_FAIL;
	}

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		MSG_BOX("Failed CPlayer Add_Component : (Com_Renderer)");
		return E_FAIL;
	}

	CCamera::CAMERADESC CameraDesc;
	CameraDesc.vEye = _float4(5.f, 5.f, 5.f, 1.f);
	CameraDesc.vAt = _float4(10.f, 0.f, 10.f, 1.f);
	CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);

	CameraDesc.fFovy = XMConvertToRadians(90.f);
	CameraDesc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 1000.f;
	CameraDesc.TransformDesc.dSpeedPerSec = m_pTransformCom->Get_Desc().dSpeedPerSec;
	CameraDesc.TransformDesc.dRotationPerSec = m_pTransformCom->Get_Desc().dRotationPerSec;

	/* For.Com_Camera */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Camera"),
		TEXT("Com_Camera"), reinterpret_cast<CComponent**>(&m_pPlayerCameraCom), &CameraDesc)))
	{
		MSG_BOX("Failed CPlayer Add_Component : (Com_Camera)");
		return E_FAIL;
	}

	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom))))
	{
		MSG_BOX("Failed CPlayer Add_Component : (Com_Collider)");
		return E_FAIL;
	}

	if (FAILED(SetUp_Collider(TEXT("../../Resources/GameData/Collider/Player.Col"))))
		return E_FAIL;

	CBounding_AABB::BOUNDINGAABBDESC VisionAABBDesc;
	VisionAABBDesc.vPosition = _float3(0.f, 0.f, 0.f);
	VisionAABBDesc.vExtents = _float3(10.f, 10.f, 10.f);
	/* For.Com_VisionCollider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_VisionCollider"), reinterpret_cast<CComponent**>(&m_pVisionColliderCom), &VisionAABBDesc)))
	{
		MSG_BOX("Failed CPlayer Add_Component : (Com_VisionCollider)");
		return E_FAIL;
	}

	CBounding_AABB::BOUNDINGAABBDESC AABBDesc;
	AABBDesc.vPosition = _float3(0.f, 0.f, 0.f);
	AABBDesc.vExtents = _float3(5.f, 3.f, 5.f);
	/* For.Com_BlockCollider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_BlockCollider"), reinterpret_cast<CComponent**>(&m_pBlockColliderCom), &AABBDesc)))
	{
		MSG_BOX("Failed CPlayer Add_Component : (Com_BlockCollider)");
		return E_FAIL;
	}

	CBounding_AABB::BOUNDINGAABBDESC BlinkAABBDesc;
	// 사용당시의 Look 기준으로 재계산 할예정.
	BlinkAABBDesc.vPosition = _float3(0.f, 0.f, 0.f);
	BlinkAABBDesc.vExtents = _float3(1.f, 1.f, 1.f);
	/* For.Com_BlinkCollider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_BlinkCollider"), reinterpret_cast<CComponent**>(&m_pBlinkColliderCom), &BlinkAABBDesc)))
	{
		MSG_BOX("Failed CPlayer Add_Component : (Com_BlinkCollider)");
		return E_FAIL;
	}

	/* For.Com_BlockEffect */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_BlockEffect"),
		TEXT("Com_BlockEffect"), reinterpret_cast<CComponent**>(&m_pBlockEffect))))
	{
		MSG_BOX("Failed CPlayer Add_Component : (Com_BlockEffect)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPlayer::Add_Component_Level(_uint iLevelIndex)
{
	CNavigation::NAVIGATIONDESC NavigationDesc;
	NavigationDesc.iCurrentIndex = 0;

	/* For.Com_Navigation */
	if (FAILED(__super::Add_Component(iLevelIndex, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NavigationDesc)))
	{
		MSG_BOX("Failed CPlayer Add_Component : (Com_Navigation)");
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CPlayer::Add_Parts(_uint iLevelIndex)
{
	CPart::PARENTMATRIXDESC ParentMatrixDesc;
	ZEROMEM(&ParentMatrixDesc);

	const CBone* pBone = m_pModelCom->Get_Bone(TEXT("Weapon_r"));

	ParentMatrixDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	ParentMatrixDesc.OffsetMatrix = pBone->Get_OffsetMatrix();
	ParentMatrixDesc.pCombindTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
	ParentMatrixDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();

	if (FAILED(__super::Add_Part(iLevelIndex, TEXT("Prototype_GameObject_Katana"), TEXT("Layer_PlayerWeapon"),
		TEXT("Part_Katana"), reinterpret_cast<CGameObject**>(&m_pKatana), &ParentMatrixDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Part(iLevelIndex, TEXT("Prototype_GameObject_Shuriken"), TEXT("Layer_PlayerWeapon"),
		TEXT("Part_Shuriken"), reinterpret_cast<CGameObject**>(&m_pShuriken), &ParentMatrixDesc)))
		return E_FAIL;

	if (FAILED(__super::Delete_Component(TEXT("Part_Shuriken"))))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(iLevelIndex, TEXT("Prototype_GameObject_Surge"), TEXT("Layer_Skill"), TEXT("GameObject_Surge"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Surge)");
		return E_FAIL;
	}

	m_pSurge = dynamic_cast<CSurge*>(pGameInstance->Find_GameObject(iLevelIndex, TEXT("Layer_Skill"), TEXT("GameObject_Surge")));

	Safe_Release(pGameInstance);

	// Wire
	ZEROMEM(&ParentMatrixDesc);
	pBone = m_pModelCom->Get_Bone(TEXT("Weapon_l"));

	ParentMatrixDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	ParentMatrixDesc.OffsetMatrix = pBone->Get_OffsetMatrix();
	ParentMatrixDesc.pCombindTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
	ParentMatrixDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();

	if (FAILED(__super::Add_Part(iLevelIndex, TEXT("Prototype_GameObject_Wire"), TEXT("Layer_PlayerProps"),
		TEXT("Part_Wire"), reinterpret_cast<CGameObject**>(&m_pWire), &ParentMatrixDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::SetUp_ShaderResources()
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

HRESULT CPlayer::Initailize_Skills()
{
	/* Dash */
	m_Dash.isUsed = false;
	m_Dash.isCoolTime = false;
	m_Dash.dCurTime = 0.0;
	m_Dash.dOriginCoolTime = 2.0;
	m_Dash.dCoolTime = 0.0;
	m_Dash.dDuration = 0.1;
	m_Dash.fLimitVelocity = 10.f;
	m_Dash.fSpeed = 60.f;

	return S_OK;
}

void CPlayer::Key_Input(_double dTimeDelta)
{
	if (GAME_OBJECT_DEAD == m_eGameEvent ||
		STATE_BLINK == m_eCurState)
		return;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// Weapons
	if (pGameInstance->Get_DIKeyState(DIK_1, CInput_Device::KEY_DOWN))
	{
		if ((STATE_IDLE == m_eCurState || STATE_RUN == m_eCurState) &&
			ANIM_LERP != m_eCurrentAnimationFlag &&
			WEAPON_KATANA != m_eCurWeapon &&
			false == m_bSwapWeapon)
		{
			m_bSwapWeapon = true;
			m_eCurWeapon = WEAPON_KATANA;
			m_eCurState = STATE_WEAPON;
		}
	}
	if (pGameInstance->Get_DIKeyState(DIK_2, CInput_Device::KEY_DOWN))
	{
		if ((STATE_IDLE == m_eCurState || STATE_RUN == m_eCurState) &&
			ANIM_LERP != m_eCurrentAnimationFlag &&
			WEAPON_SHURIKEN != m_eCurWeapon &&
			false == m_bSwapWeapon)
		{
			m_bSwapWeapon = true;
			m_eCurWeapon = WEAPON_SHURIKEN;
			m_eCurState = STATE_WEAPON;
		}
	}

	// straight
	if (pGameInstance->Get_DIKeyState(DIK_W))
	{
		if (STATE_IDLE == m_eCurState && 
			false == m_pTransformCom->IsJumping() &&
			STATE_ATTACK != m_eCurState)
		{
			m_eCurState = STATE_RUN;
		}
		
		m_pTransformCom->Go_Straight(dTimeDelta);
	}
	if (pGameInstance->Get_DIKeyState(DIK_W, CInput_Device::KEY_UP))
	{
		// Up누를때 IDLE로 강제로 바꿔주면 처리에 문제생김.
		if (STATE_ATTACK != m_eCurState &&
			STATE_WEAPON != m_eCurState)
			m_eCurState = STATE_IDLE;
	}
	// backward
	if (pGameInstance->Get_DIKeyState(DIK_S))
	{
		if (STATE_IDLE == m_eCurState && 
			false == m_pTransformCom->IsJumping() &&
			STATE_ATTACK != m_eCurState)
		{
			m_eCurState = STATE_RUN;
		}

		m_pTransformCom->Go_Backward(dTimeDelta);
	}
	if (pGameInstance->Get_DIKeyState(DIK_S, CInput_Device::KEY_UP))
	{
		if (STATE_ATTACK != m_eCurState &&
			STATE_WEAPON != m_eCurState)
			m_eCurState = STATE_IDLE;
	}
	// left
	if (pGameInstance->Get_DIKeyState(DIK_A))
	{
		if (STATE_IDLE == m_eCurState &&
			false == m_pTransformCom->IsJumping() &&
			STATE_ATTACK != m_eCurState)
		{
			m_eCurState = STATE_RUN;
		}

		m_pTransformCom->Go_Left(dTimeDelta);
	}
	if (pGameInstance->Get_DIKeyState(DIK_A, CInput_Device::KEY_UP))
	{
		if (STATE_ATTACK != m_eCurState &&
			STATE_WEAPON != m_eCurState)
			m_eCurState = STATE_IDLE;
	}
	// right
	if (pGameInstance->Get_DIKeyState(DIK_D))
	{
		if (STATE_IDLE == m_eCurState &&
			false == m_pTransformCom->IsJumping() &&
			STATE_ATTACK != m_eCurState)
		{
			m_eCurState = STATE_RUN;
		}

		m_pTransformCom->Go_Right(dTimeDelta);
	}
	if (pGameInstance->Get_DIKeyState(DIK_D, CInput_Device::KEY_UP))
	{
		if (STATE_ATTACK != m_eCurState &&
			STATE_WEAPON != m_eCurState)
			m_eCurState = STATE_IDLE;
	}
	// jump, SPACE
	if (false == m_pTransformCom->IsJumping() && pGameInstance->Get_DIKeyState(DIK_SPACE, CInput_Device::KEY_DOWN))
	{
		m_pTransformCom->Jump(m_fJumpPower, dTimeDelta);
		if (STATE_ATTACK != m_eCurState)
			m_eCurState = STATE_IDLE;
	}
	// dash, Lshift
	if (pGameInstance->Get_DIKeyState(DIK_LSHIFT, CInput_Device::KEY_DOWN))
	{
		m_Dash.isUsed = true;
		m_Dash.dCurTime = 0.0;
	}
	// Crouch, Lcontrol
	if (pGameInstance->Get_DIKeyState(DIK_LCONTROL))
	{
		if (STATE_ATTACK != m_eCurState)
			m_eCurState = STATE_CROUCH;
		m_isCrouch = true;
	}
	if (pGameInstance->Get_DIKeyState(DIK_LCONTROL, CInput_Device::KEY_UP))
	{
		if (STATE_ATTACK != m_eCurState &&
			STATE_WEAPON != m_eCurState)
			m_eCurState = STATE_IDLE;
		m_isCrouch = false;
	}
	// Hook
	if (STATE_HOOK != m_eCurState && pGameInstance->Get_DIMouseState(CInput_Device::DIMK_RBUTTON, CInput_Device::KEY_DOWN))
	{
		m_eCurState = STATE_HOOK;
		if (true == Check_Hook(dTimeDelta))
			pGameInstance->Play_Sound(TEXT("Hook.ogg"), CSound_Manager::SOUND_HOOK, 0.3f, true);
	}
	// attack, Lbutton, Lclick
	if (pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_DOWN))
	{
		if (0 < m_BlockEnemyWeapons.size() && 
			STATE_BLOCK != m_eCurState &&
			STATE_ATTACK != m_eCurState &&
			STATE_WEAPON != m_eCurState &&
			STATE_BLINK != m_eCurState)
		{
			m_isBlocked = true;
			m_eCurState = STATE_BLOCK;
		}
		// 막기가 안나갔을 경우 일반 공격판정.
		else
		{
			if (STATE_ATTACK != m_eCurState &&
				STATE_CLIMB != m_eCurState &&
				STATE_WEAPON != m_eCurState &&
				STATE_BLINK != m_eCurState)
			{
				m_eCurState = STATE_ATTACK;
			}
			
			if (STATE_ATTACK == m_eCurState &&
				WEAPON_SHURIKEN == m_eCurWeapon)
			{
				m_pShuriken->Attack(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)));
				pGameInstance->Play_Sound(TEXT("Shuriken_Default.ogg"), CSound_Manager::SOUND_SWORD, 0.3f, true);
			}
			
			if (STATE_ATTACK == m_eCurState &&
				WEAPON_KATANA == m_eCurWeapon &&
				m_ePreState != m_eCurState)
			{
				pGameInstance->Play_Sound(TEXT("Sword_Default.ogg"), CSound_Manager::SOUND_SWORD, 0.3f, true);
			}
			
			XMStoreFloat3(&m_vAttackPositon, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

			if (STATE_ATTACK == m_eCurState &&
				ANIM_LERP != m_eCurrentAnimationFlag)
			{
				// 실제 게임이랑 속도 맞춤.
				// 40퍼센트가 넘으면 다음 공격 실행가능 (연속공격 처리.)
				if (0.35f <= m_pModelCom->Get_CurrentFramePercent())
				{
					_uint iCurrnetAnimIndex = m_pModelCom->Get_CurrentAnimIndex();
					// if Attack Right
					if (84 == iCurrnetAnimIndex ||
						85 == iCurrnetAnimIndex ||
						86 == iCurrnetAnimIndex)
						m_pModelCom->Set_AnimIndex(87 + rand() % 3, false);
					else if (87 == iCurrnetAnimIndex ||
						88 == iCurrnetAnimIndex ||
						89 == iCurrnetAnimIndex)
						m_pModelCom->Set_AnimIndex(84 + rand() % 3, false);

					pGameInstance->Play_Sound(TEXT("Sword_Default.ogg"), CSound_Manager::SOUND_SWORD, 0.3f, true);
				}
			}
		}
	}

	// Time Slow
	if (pGameInstance->Get_DIKeyState(DIK_Z, CInput_Device::KEY_DOWN))
	{
		m_pTransformCom->ZeroVelocity();
	}
	if (pGameInstance->Get_DIKeyState(DIK_Z))
	{
		pGameInstance->Set_SlowedTime(TEXT("MainTimer"), 0.1);
	}
	if (pGameInstance->Get_DIKeyState(DIK_Z, CInput_Device::KEY_UP))
	{
		pGameInstance->Set_SlowedTime(TEXT("MainTimer"), 1.0);
	}

	// Swap Skill
	if (pGameInstance->Get_DIKeyState(DIK_TAB, CInput_Device::KEY_DOWN))
	{
		if (SKILL_BLINK == m_eCurrentSkill)
			m_eCurrentSkill = SKILL_SURGE;
		else if (SKILL_SURGE == m_eCurrentSkill)
			m_eCurrentSkill = SKILL_BLINK;
	}
	// Use Skill
	if (pGameInstance->Get_DIKeyState(DIK_Q, CInput_Device::KEY_DOWN))
	{
		if (m_iSkillStack >= m_iSkillMaxStack)
		{
			if (SKILL_BLINK == m_eCurrentSkill)
			{
				m_eCurState = STATE_BLINK;
				m_iSkillStack = 0;
			}
			else if (SKILL_SURGE == m_eCurrentSkill)
			{
				_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				m_pSurge->Fire(vPos, vPos + m_pTransformCom->Get_State(CTransform::STATE_LOOK));
				m_eCurState = STATE_ATTACK;
				m_iSkillStack = 0;
			}
		}
	}

	_long		dwMouseMove = 0;

#ifdef _DEBUG
	if (false == m_isMouseFixed)
	{
		Safe_Release(pGameInstance);
		return;
	}
	if (pGameInstance->Get_DIKeyState(DIK_F2, CInput_Device::KEY_DOWN))
	{
		if (m_isInvisible)
			m_isInvisible = false;
		else
			m_isInvisible = true;
	}
#endif // _DEBUG

	// 무조건 카메라 버티컬먼저 작동해야함.
	// 안그러면 카메라와 실제 모델간에 각도값이 점점 틀어짐
	// -> 캠Right를 기준으로 회전하기때문으로 보임.
	// Mouse Move Vertical
	if (dwMouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMM_Y))
	{
		// 회전축을 플레이어의 카메라 right 벡터 기준으로 처리.
		_vector	vCamRight = XMVector3Normalize(m_pPlayerCameraCom->Get_TransformState(CTransform::STATE_RIGHT));

		m_pTransformCom->Turn(vCamRight, dwMouseMove * m_fMouseSensitivity, dTimeDelta);

		_vector vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);

		if (XMConvertToRadians(15.f) > XMVectorGetX(XMVector3Dot(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMVector3Normalize(vUp))))
		{
			m_pTransformCom->Turn(vCamRight, -1.f * dwMouseMove * m_fMouseSensitivity, dTimeDelta);
		}
	}
	// Mouse Move Horizontal
	if (dwMouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMM_X))
	{
		_vector	vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

		m_pTransformCom->Turn(vUp, dwMouseMove * m_fMouseSensitivity, dTimeDelta);
	}

	Safe_Release(pGameInstance);
}

void CPlayer::Fix_Mouse()
{
	if (GAME_OBJECT_DEAD == m_eGameEvent)
		return;

#ifdef _DEBUG
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	// 키보드 ` 키
	if (pGameInstance->Get_DIKeyState(DIK_GRAVE, CInput_Device::KEY_DOWN))
	{
		if (m_isMouseFixed)
			m_isMouseFixed = false;
		else
			m_isMouseFixed = true;
	}

	Safe_Release(pGameInstance);

	if (false == m_isMouseFixed)
		return;
#endif // _DEBUG

	POINT	ptMouse{ g_iWinSizeX >> 1, g_iWinSizeY >> 1 };

	ClientToScreen(g_hWnd, &ptMouse);
	SetCursorPos(ptMouse.x, ptMouse.y);
}

void CPlayer::AnimationState(_double dTimeDelta)
{
	m_eCurrentAnimationFlag = m_pModelCom->Get_AnimationState();

	// 애니메이션이 종료 되었을때 따로 처리되는 명령이 없을 경우 IDLE처리
	if (ANIM_FINISHED == m_eCurrentAnimationFlag &&
		m_ePreState == m_eCurState)
	{
		m_eCurState = STATE_IDLE;
	}

	Motion_Change(m_eCurrentAnimationFlag);

	m_pModelCom->Play_Animation(dTimeDelta);
}

void CPlayer::Motion_Change(ANIMATIONFLAG eAnimationFlag)
{
	// 모션 변경 처리 전 예외처리.
	if (false == (ANIM_PLAYING == eAnimationFlag || ANIM_FINISHED == eAnimationFlag))
		return;
	if (GAME_OBJECT_DEAD == m_eGameEvent)
		m_eCurState = STATE_DEAD;

	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case STATE_IDLE:
			if (WEAPON_KATANA == m_eCurWeapon)
				m_pModelCom->Set_AnimIndex(95);
			else if (WEAPON_SHURIKEN == m_eCurWeapon)
				m_pModelCom->Set_AnimIndex(142);
			break;
		case STATE_ATTACK:
			if (WEAPON_KATANA == m_eCurWeapon)
				m_pModelCom->Set_AnimIndex(84 + rand() % 6, false);
			else if (WEAPON_SHURIKEN == m_eCurWeapon)
				m_pModelCom->Set_AnimIndex(129, false);
			break;
		case STATE_RUN:
			if (WEAPON_KATANA == m_eCurWeapon)
				m_pModelCom->Set_AnimIndex(102);
			else if (WEAPON_SHURIKEN == m_eCurWeapon)
				m_pModelCom->Set_AnimIndex(132);
			break;
		case STATE_RUNWALL_L:
			if (WEAPON_KATANA == m_eCurWeapon)
				m_pModelCom->Set_AnimIndex(103);
			else if (WEAPON_SHURIKEN == m_eCurWeapon)
				m_pModelCom->Set_AnimIndex(145);
			break;
		case STATE_RUNWALL_R:
			if (WEAPON_KATANA == m_eCurWeapon)
				m_pModelCom->Set_AnimIndex(104);
			else if (WEAPON_SHURIKEN == m_eCurWeapon)
				m_pModelCom->Set_AnimIndex(146);
			break;
		case STATE_CROUCH:
			if (WEAPON_KATANA == m_eCurWeapon)
				m_pModelCom->Set_AnimIndex(92);
			else if (WEAPON_SHURIKEN == m_eCurWeapon)
				m_pModelCom->Set_AnimIndex(131);
			break;
		case STATE_HOOK:
			m_pModelCom->Set_AnimIndex(62, false);
			break;
		case STATE_BLOCK:
			if (WEAPON_KATANA == m_eCurWeapon)
				m_pModelCom->Set_AnimIndex(78 + rand() % 3, false);
			else if (WEAPON_SHURIKEN == m_eCurWeapon)
				m_pModelCom->Set_AnimIndex(130, false);
			break;
		case STATE_WEAPON:
			if (WEAPON_KATANA == m_eCurWeapon)
				m_pModelCom->Set_AnimIndex(148, false);
			else if (WEAPON_SHURIKEN == m_eCurWeapon)
				m_pModelCom->Set_AnimIndex(147, false);
			break;
		case STATE_DEAD:
			m_pModelCom->Set_AnimIndex(169 + rand() % 10, false);
			break;
		case STATE_BLINK:
			m_pModelCom->Set_AnimIndex(118, false);
			break;
		}

		m_ePreState = m_eCurState;
	}
}

void CPlayer::WallRunCameraReset(_double dTimeDelta)
{
	if (false == m_isWallRun &&
		0.1f <= fabs(m_fWallRunAngle))
	{
		if (!XMVectorGetX(XMVectorEqual(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)), XMVectorSet(0.f, 0.f, 1.f, 0.f))))
		{
			if (m_fWallRunAngle > 1.f)
				m_fWallRunAngle -= 60.f * (_float)dTimeDelta;
			else if (m_fWallRunAngle < -1.f)
				m_fWallRunAngle += 60.f * (_float)dTimeDelta;
			else
				m_fWallRunAngle = 0.f;

			m_eCurState = STATE_IDLE;

			m_pPlayerCameraCom->Rotation(XMLoadFloat3(&m_vWallDir), XMConvertToRadians(m_fWallRunAngle), true);
		}
	}
}

void CPlayer::Attack(_double dTimeDelta)
{
	if (STATE_ATTACK != m_eCurState ||
		84 > m_pModelCom->Get_CurrentAnimIndex() ||
		90 <= m_pModelCom->Get_CurrentAnimIndex())
		return;

	if (WEAPON_KATANA == m_eCurWeapon)
		m_pKatana->Add_TrailRender();

	_float fAnimFramePercent = m_pModelCom->Get_CurrentFramePercent();

	if (0.15f > fAnimFramePercent)
	{
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

		_float fDist = 30.f;
		_bool isChecked = false;

		_vector vClosedEnemyPos;

		_vector vPointLook = vLook * fDist;

		for (auto& pEnemy : m_InRangeEnemyColliders)
		{
			CComposite* pEnemyCollOwner = pEnemy->Get_Owner();
			if (static_cast<CGameObject*>(pEnemyCollOwner)->Is_Dead())
				continue;
			_float3 vEnemyPos = pEnemy->Get_BoundingCenterPosition();
			_float fPointDist = XMVectorGetX(XMVector3LinePointDistance(vPos, vPos + vPointLook, XMLoadFloat3(&vEnemyPos)));

			if (fDist >= fPointDist)
			{
				vClosedEnemyPos = XMLoadFloat3(&vEnemyPos);
				fDist = fPointDist;
				isChecked = true;
			}
		}

		if (true == isChecked)
		{
			if (false == m_pTransformCom->Chase(vClosedEnemyPos - vLook * 3.f, dTimeDelta * 3.0, 3.f))
				m_pTransformCom->ZeroVelocity();
		}
	}

	if (0.15f <= fAnimFramePercent && 0.5f > fAnimFramePercent)
	{
		if (WEAPON_KATANA == m_eCurWeapon)
			m_pKatana->Attack();
	}
}

void CPlayer::Block(_double dTimeDelta)
{
	if (STATE_BLOCK != m_eCurState ||
		 0 == m_BlockEnemyWeapons.size())
		return;

	if (0.7f < m_pModelCom->Get_CurrentFramePercent())
	{
		m_BlockEnemyWeapons.clear();
		return;
	}
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	
	for (auto iter = m_BlockEnemyWeapons.begin(); iter != m_BlockEnemyWeapons.end();)
	{
		if (wcswcs((*iter)->Get_Tag(), TEXT("Bullet")))
		{
			CBullet* pBullet = static_cast<CBullet*>((const_cast<CGameObject*>(*iter)));
			pBullet->Set_Type(COLLISIONDESC::COLTYPE_PLAYERWEAPON);
			pBullet->Set_LayerTag(TEXT("Layer_PlayerWeapon"));
			_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			pBullet->Fire(vPos, vPos + vLook);
			pGameInstance->Play_Sound(TEXT("Parrying_Bullet.ogg"), CSound_Manager::SOUND_SWORD, 0.3f, true);
			iter = m_BlockEnemyWeapons.erase(iter);
		}
		else if (wcswcs((*iter)->Get_Tag(), TEXT("Sword")))
		{
			CSword* pSword = static_cast<CSword*>((const_cast<CGameObject*>(*iter)));
			pSword->Blocked();
			pGameInstance->Play_Sound(TEXT("Parrying_Sword.ogg"), CSound_Manager::SOUND_SWORD, 0.4f, true);
			iter = m_BlockEnemyWeapons.erase(iter);
		}
		else
		{
			// 여기에 걸리지않았을 경우 그냥 어디에 맞아서 사라졌을 확률이 높기 때문에 그냥 삭제해준다.
			iter = m_BlockEnemyWeapons.erase(iter);
		}
	}
	
	Safe_Release(pGameInstance);
	
	_float4 vWeaponPos;
	if (WEAPON_SHURIKEN == m_eCurWeapon)
		memcpy(&vWeaponPos, m_pShuriken->Get_CombinedWorldMatrix().m[3], sizeof(_float4));
	else
		memcpy(&vWeaponPos, m_pKatana->Get_CombinedWorldMatrix().m[3], sizeof(_float4));

	m_pBlockEffect->Render_Effect(XMLoadFloat4(&vWeaponPos) + m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	m_pPlayerCameraCom->Shake_RollPitchYaw(0.f, 10.f, 0.f, 4.0, 0.2);
}

void CPlayer::Add_Collisions()
{
	_matrix VisionMatrix = XMMatrixTranslation(0.f, 0.f, 10.f);
	_matrix BlockMatrix = XMMatrixTranslation(0.f, 0.f, 3.f);
	_matrix BlinkMatrix = XMMatrixTranslation(0.f, 0.f, 35.f);
	// 충돌처리
	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
	m_pVisionColliderCom->Tick(VisionMatrix * m_pTransformCom->Get_WorldMatrix());
	m_pBlockColliderCom->Tick(BlockMatrix * m_pTransformCom->Get_WorldMatrix());
	CBounding_AABB::BOUNDINGAABBDESC AABBDesc;
	ZEROMEM(&AABBDesc);
	AABBDesc.vPosition = _float3(0.f, 0.f, 0.f);
	AABBDesc.vExtents = _float3(35.f, 25.f, 35.f);
	m_pBlinkColliderCom->Set_BoundingDesc(&AABBDesc);
	m_pBlinkColliderCom->Tick(BlinkMatrix * m_pTransformCom->Get_WorldMatrix());

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	pGameInstance->Add_Collider(COLLISIONDESC::COLTYPE_PLAYER, m_pColliderCom);
	pGameInstance->Add_Collider(COLLISIONDESC::COLTYPE_PLAYERVISION, m_pVisionColliderCom);
	pGameInstance->Add_Collider(COLLISIONDESC::COLTYPE_PLAYERVISION, m_pBlockColliderCom);

	if (STATE_BLINK == m_eCurState)
	{
		pGameInstance->Add_Collider(COLLISIONDESC::COLTYPE_PLAYERVISION, m_pBlinkColliderCom);
	}

	Safe_Release(pGameInstance);
}

void CPlayer::CollisionStayWall(COLLISIONDESC CollisionDesc)
{
	_vector vWallPos = CollisionDesc.pOtherTransform->Get_State(CTransform::STATE_POSITION);
	// Right 기본이 -z
	XMStoreFloat3(&m_vWallDir, XMVector3Normalize(CollisionDesc.pOtherTransform->Get_State(CTransform::STATE_RIGHT)));
	// Look 기본이 +x
	_vector vWallLook = XMVector3Normalize(CollisionDesc.pOtherTransform->Get_State(CTransform::STATE_LOOK));
	_vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos.m128_f32[1] = 0.f;
	vWallPos.m128_f32[1] = 0.f;

	_vector vDir = vWallPos - vPos;

	if (0 < XMVectorGetX(XMVector3Dot(XMVector3Normalize(vDir), XMVector3Normalize(vWallLook))))
	{
		if (-15.f < m_fWallRunAngle)
			m_fWallRunAngle -= 60.f * (_float)g_TimeDelta;
		
		if (STATE_ATTACK != m_eCurState)
		{
			if (0 < XMVectorGetX(XMVector3Dot(vLook, XMLoadFloat3(&m_vWallDir))))
				m_eCurState = STATE_RUNWALL_L;
			else
				m_eCurState = STATE_RUNWALL_R;
		}
		
		// Turn
		m_pPlayerCameraCom->Rotation(XMLoadFloat3(&m_vWallDir), XMConvertToRadians(m_fWallRunAngle), true);
	}
	else
	{
		if (15.f > m_fWallRunAngle)
			m_fWallRunAngle += 60.f * (_float)g_TimeDelta;

		if (STATE_ATTACK != m_eCurState)
		{
			if (0 < XMVectorGetX(XMVector3Dot(vLook, XMLoadFloat3(&m_vWallDir))))
				m_eCurState = STATE_RUNWALL_R;
			else
				m_eCurState = STATE_RUNWALL_L;
		}

		m_pPlayerCameraCom->Rotation(XMLoadFloat3(&m_vWallDir), XMConvertToRadians(m_fWallRunAngle), true);
	}

	if (0 < XMVectorGetX(XMVector3Dot(vLook, XMLoadFloat3(&m_vWallDir))))
		m_pTransformCom->WallRun(m_fWallRunY, XMLoadFloat3(&m_vWallDir) * m_fWallRunVelocity);
	else
		m_pTransformCom->WallRun(m_fWallRunY, XMLoadFloat3(&m_vWallDir) * -m_fWallRunVelocity);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	
	// 벽점프
	if (pGameInstance->Get_DIKeyState(DIK_SPACE, CInput_Device::KEY_DOWN))
	{
		m_fWallRunY = 0.f;
		m_isWallRun = false;
		if (0 > m_fWallRunAngle)
		{
			m_pTransformCom->Jump(-vWallLook + XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fJumpPower, g_TimeDelta);
		}
		else
		{
			m_pTransformCom->Jump(vWallLook + XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fJumpPower, g_TimeDelta);
		}
	}

	Safe_Release(pGameInstance);
}

_bool CPlayer::Check_Hook(_double dTimeDelta)
{
	if (STATE_HOOK != m_eCurState &&
		62 != m_pModelCom->Get_CurrentAnimIndex())
		return false;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	_float4 vRayPos = _float4(0.f, 0.f, 0.f, 1.f), vRayDir = _float4(0.f, 0.f, 0.f, 0.f);

	if (FAILED(pGameInstance->Get_WorldMouseRay(m_pContext, g_hWnd, &vRayPos, &vRayDir)))
	{
		Safe_Release(pGameInstance);
		return false;
	}

	Safe_Release(pGameInstance);

	CLayer* pHookLayer = pGameInstance->Find_Layer(LEVEL_STAGE1, TEXT("Layer_Hook"));

	if (nullptr == pHookLayer)
		return false;

	_float fDist = 90.f;

	for (auto& pObject : pHookLayer->Get_AllGameObject())
	{
		CCollider* pCollider = static_cast<CColProp*>(pObject.second)->Get_Collider();
		if (nullptr == pCollider)
			continue;

		if (true == pCollider->RayIntersects(XMLoadFloat4(&vRayPos), XMLoadFloat4(&vRayDir), fDist))
		{
			if (80.f > fDist)
			{
				_float3 vHookCenter = pCollider->Get_BoundingCenterPosition();
				m_pWire->Render_Wire(XMVectorSet(vHookCenter.x, vHookCenter.y, vHookCenter.z, 1.f), 0.3);
				m_pTransformCom->Jump(XMVector4Normalize(XMLoadFloat4(&vRayDir)), fDist * m_fHookPower, dTimeDelta);
				return true;
			}
		}
	}

	return false;
}

void CPlayer::Tick_Skills(_double dTimeDelta)
{
	Dash(dTimeDelta);
	Blink(dTimeDelta);
}

void CPlayer::CameraMove(_double dTimeDelta)
{
	if (STATE_ATTACK != m_eCurState)
		return;

	_float fCurrentFramePercent = m_pModelCom->Get_CurrentFramePercent();
	if (0.1f < fCurrentFramePercent)
		return;

	/*
	84 - Att R1 down
	85 - Att R2 - 
	86 - Att R3 up 
	87 - Att L1 down
	88 - Att L2 - 
	89 - Att L3 up
	*/

	enum ATTNUM
	{
		RDOWN = 84,
		RNORM,
		RUP,
		LDOWN,
		LNORM,
		LUP,
	};

	_float fVerticalMove = 0.f;
	_float fHorizentalMove = 0.f;

	_uint iCurrentAnimIndex = m_pModelCom->Get_CurrentAnimIndex();

	switch (iCurrentAnimIndex)
	{
	case RDOWN:
		fVerticalMove = m_fAttackCamMove;
		fHorizentalMove = -m_fAttackCamMove;
		break;
	case RNORM:
		fVerticalMove = 0.f;
		fHorizentalMove = -m_fAttackCamMove;
		break;
	case RUP:
		fVerticalMove = -m_fAttackCamMove;
		fHorizentalMove = -m_fAttackCamMove;
		break;
	case LDOWN:
		fVerticalMove = m_fAttackCamMove;
		fHorizentalMove = m_fAttackCamMove;
		break;
	case LNORM:
		fVerticalMove = 0.f;
		fHorizentalMove = m_fAttackCamMove;
		break;
	case LUP:
		fVerticalMove = -m_fAttackCamMove;
		fHorizentalMove = m_fAttackCamMove;
		break;
	default:
		break;
	}

	_vector	vCamRight = XMVector3Normalize(m_pPlayerCameraCom->Get_TransformState(CTransform::STATE_RIGHT));
	m_pTransformCom->Turn(vCamRight, fVerticalMove * m_fMouseSensitivity, dTimeDelta);
	_vector vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);

	if (XMConvertToRadians(30.f) > XMVectorGetX(XMVector3Dot(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMVector3Normalize(vUp))))
	{
		m_pTransformCom->Turn(vCamRight, -1.f * fVerticalMove * m_fMouseSensitivity, dTimeDelta);
	}

	_vector	vAxisUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	m_pTransformCom->Turn(vAxisUp, fHorizentalMove * m_fMouseSensitivity, dTimeDelta);
}

void CPlayer::CameraOffset(_double dTimeDelta)
{
	_float4 vEye, vAt;

	// 위치값은 고정
	XMStoreFloat4(&vEye, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	XMStoreFloat4(&vAt, XMLoadFloat4(&vEye) + m_pTransformCom->Get_State(CTransform::STATE_LOOK));

	m_pPlayerCameraCom->Set_LookAtLH(vEye, vAt);
}

void CPlayer::Dash(_double dTimeDelta)
{
	// 스킬 현재 시간 초기화는 사용하였을때 딱한번 처리.
	if (0.0 < m_Dash.dCoolTime)
	{
		// 쿨타임이라 사용 불가.
		m_Dash.dCoolTime -= dTimeDelta;
		m_Dash.isCoolTime = true;
		// shift 미리눌림방지
		m_Dash.isUsed = false;
	}
	else
		m_Dash.isCoolTime = false;
	// 누르질 않음.
	if (false == m_Dash.isUsed)
		return;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	pGameInstance->Play_Sound(TEXT("Dash.ogg"), CSound_Manager::SOUND_DASH, 0.3f);
	Safe_Release(pGameInstance);
	
	m_Dash.dCurTime += dTimeDelta;
	m_pTransformCom->Set_LimitVelocity(m_Dash.fLimitVelocity);
	m_pTransformCom->Set_Speed(m_Dash.fSpeed);

	if (m_Dash.dCurTime >= m_Dash.dDuration)
	{
		// 초기화
		m_pTransformCom->Set_LimitVelocity(1.f);
		m_pTransformCom->Set_Speed(m_fSpeed);
		m_Dash.isUsed = false;
		m_Dash.dCurTime = 0.0;
		m_Dash.dCoolTime = m_Dash.dOriginCoolTime;
	}
}

void CPlayer::Blink(_double dTimeDelta)
{
	if (STATE_BLINK != m_eCurState ||
		0 == m_BlinkEnemys.size())
		return;

	// 시간값을 1/10으로 줄였으므로 10배  처리해서 시간을 더해줘야함.
	m_dBlinkTimeAcc += dTimeDelta * 10.0;

	_float fFOV = m_pPlayerCameraCom->Get_FOV();
	m_pPlayerCameraCom->Set_FOV(fFOV - (_float)dTimeDelta);
	m_pRendererCom->Set_RedScale(true);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	
	if (false == m_isBlink)
		pGameInstance->Play_Sound(TEXT("Blink_Charge.ogg"), CSound_Manager::SOUND_SKILL, 0.3f);

	pGameInstance->Set_SlowedTime(TEXT("MainTimer"), 0.1);
	m_pTransformCom->ZeroVelocity();
	m_isBlink = true;

	// 시간초과
	if (m_dBlinkTimeAcc > m_dBlinkTime)
	{
		m_pRendererCom->Set_RedScale(false);
		m_pPlayerCameraCom->Set_FOV(XMConvertToRadians(90.f));
		m_eCurState = STATE_IDLE;
		pGameInstance->Set_SlowedTime(TEXT("MainTimer"), 1.0);
		m_BlinkEnemys.clear();
		m_isBlink = false;
		m_dBlinkTimeAcc = 0.0;
		Safe_Release(pGameInstance);
		return;
	}

	if (pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_DOWN))
	{
		m_pRendererCom->Set_RedScale(false);
		m_pPlayerCameraCom->Set_FOV(XMConvertToRadians(90.f));
		m_isBlink = false;

		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

		_float fDist = 0.f;
		_vector vFarEnemyPos;

		for (auto& pEnemy : m_BlinkEnemys)
		{
			_vector vEnemyPos = pEnemy->Get_Transform()->Get_State(CTransform::STATE_POSITION);
			_float fLength = XMVectorGetX(XMVector3Length(vEnemyPos - vPos));

			if (fDist < fLength)
			{
				vFarEnemyPos = vEnemyPos;
				fDist = fLength;
			}
			const_cast<CGameObject*>(pEnemy)->Set_GameEvent(GAME_OBJECT_DEAD);
		}

		if (0.1f < fDist)
		{
			m_pTransformCom->Jump(vLook, fDist + 5.f, dTimeDelta);
		}

		m_eCurState = STATE_ATTACK;
		pGameInstance->Play_Sound(TEXT("Blink.ogg"), CSound_Manager::SOUND_SKILL, 0.3f, true);

		// Finish
		m_dBlinkTimeAcc = 0.0;
		pGameInstance->Set_SlowedTime(TEXT("MainTimer"), 1.0);
		m_BlinkEnemys.clear();
	}

	Safe_Release(pGameInstance);
}

HRESULT CPlayer::SetUp_AnimationNotifies(const _tchar* pNotifyFilePath)
{
	// 몇번 애니메이션 인덱스에 어떤 파일 경로를 넣을건지
	// 아니면 파일 저장자체를 (인덱스, 벡터<노티파이구조체> 정보) 이런식으로 감싸서 저장하고
	// 여기서 Read file해서 "특정 애니메이션 인덱스", "벡터" 를 인자값으로 넘겨주자
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

void CPlayer::SwapWeapon()
{
	if (STATE_WEAPON != m_eCurState ||
		(147 != m_pModelCom->Get_CurrentAnimIndex() && 148 != m_pModelCom->Get_CurrentAnimIndex()))
		return;

	_float fCurrentFramePercent = m_pModelCom->Get_CurrentFramePercent();

	if (0.5f > fCurrentFramePercent)
		return;
	if (false == m_bSwapWeapon)
		return;

	if (WEAPON_KATANA == m_eCurWeapon)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);
		pGameInstance->Play_Sound(TEXT("Change_Sword.ogg"), CSound_Manager::SOUND_SWORD, 0.3f, true);
		Safe_Release(pGameInstance);

		if (FAILED(__super::Delete_Component(TEXT("Part_Shuriken"))))
			return;

		m_Components.emplace(TEXT("Part_Katana"), m_pKatana);
		Safe_AddRef(m_pKatana);
		m_bSwapWeapon = false;
	}

	if (WEAPON_SHURIKEN == m_eCurWeapon)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);
		pGameInstance->Play_Sound(TEXT("Change_Shuriken.ogg"), CSound_Manager::SOUND_SWORD, 0.3f, true);
		Safe_Release(pGameInstance);
		
		if (FAILED(__super::Delete_Component(TEXT("Part_Katana"))))
			return;

		m_Components.emplace(TEXT("Part_Shuriken"), m_pShuriken);
		Safe_AddRef(m_pShuriken);
		m_bSwapWeapon = false;
	}
}

void CPlayer::Check_Fall()
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	// 현재 Y값이 셀의 Y값보다 오프셋만큼 적어질경우 떨어진것으로 판단함.
	if (XMVectorGetY(vPos) < m_pNavigationCom->Get_CurrentCellY(vPos) - 25.f)
		m_eGameEvent = GAME_OBJECT_DEAD;
}

HRESULT CPlayer::Add_Notifies()
{
	// Notify SetUp
	if (FAILED(SetUp_AnimationNotifies(TEXT("../../Resources/GameData/Notify/Att_R1.Notify"))))
		return E_FAIL;
	if (FAILED(SetUp_AnimationNotifies(TEXT("../../Resources/GameData/Notify/Run.Notify"))))
		return E_FAIL;
	if (FAILED(SetUp_AnimationNotifies(TEXT("../../Resources/GameData/Notify/WallRun_L.Notify"))))
		return E_FAIL;
	if (FAILED(SetUp_AnimationNotifies(TEXT("../../Resources/GameData/Notify/WallRun_R.Notify"))))
		return E_FAIL;
	if (FAILED(SetUp_AnimationNotifies(TEXT("../../Resources/GameData/Notify/SH_Run.Notify"))))
		return E_FAIL;
	if (FAILED(SetUp_AnimationNotifies(TEXT("../../Resources/GameData/Notify/SH_WallRun_L.Notify"))))
		return E_FAIL;
	if (FAILED(SetUp_AnimationNotifies(TEXT("../../Resources/GameData/Notify/SH_WallRun_R.Notify"))))
		return E_FAIL;
	/*if (FAILED(SetUp_AnimationNotifies(TEXT("../../Resources/GameData/Notify/Hook_Pull.Notify"))))
		return E_FAIL;*/

	return S_OK;
}

void CPlayer::SoundState(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (STATE_RUN == m_eCurState)
	{
		if (true == pGameInstance->Timer(0.25, dTimeDelta) &&
			false == m_pTransformCom->IsJumping())
		{
			pGameInstance->Play_Sound(TEXT("Step.ogg"), CSound_Manager::SOUND_STEP, 0.3f, true);
		}
	}
	else if (STATE_CROUCH == m_eCurState)
	{
		if (CELL_SLIDE == m_pNavigationCom->Get_CurrentCellFlag())
			pGameInstance->Play_Sound(TEXT("Slide.ogg"), CSound_Manager::SOUND_STEP, 0.3f);
	}
	else
		pGameInstance->Stop_Sound(CSound_Manager::SOUND_STEP);

	Safe_Release(pGameInstance);
}

HRESULT CPlayer::SetUp_Collider(const _tchar* pColliderFilePath)
{
	HANDLE hFile = CreateFile(pColliderFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	// 콜라이더에도 피벗을 적용시켜줘야함.
	// 툴에서 적용한건 그냥 로컬에 적용한거라 좀 다름.
	// 특히 트랜스폼이 피벗으로 들어갔을경우 문제가 있을 수 있음.
	_matrix PivotMatrix = m_pModelCom->Get_PivotMatrix();

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong	dwByte = 0;

	CCollider::TYPE eType;
	// Current Animation Index
	ReadFile(hFile, &eType, sizeof(CCollider::TYPE), &dwByte, nullptr);

	switch (eType)
	{
	case CCollider::TYPE_SPHERE:
		CBounding_Sphere::BOUNDINGSPHEREDESC SphereDesc;
		ReadFile(hFile, &SphereDesc, sizeof(CBounding_Sphere::BOUNDINGSPHEREDESC), &dwByte, nullptr);
		m_pColliderCom->Set_BoundingDesc(&SphereDesc);
		break;
	case CCollider::TYPE_AABB:
		CBounding_AABB::BOUNDINGAABBDESC AABBDesc;
		ReadFile(hFile, &AABBDesc, sizeof(CBounding_AABB::BOUNDINGAABBDESC), &dwByte, nullptr);
		XMStoreFloat3(&AABBDesc.vPosition, XMVector3TransformCoord(XMLoadFloat3(&AABBDesc.vPosition), PivotMatrix));
		m_pColliderCom->Set_BoundingDesc(&AABBDesc);
		break;
	case CCollider::TYPE_OBB:
		CBounding_OBB::BOUNDINGOBBDESC OBBDesc;
		ReadFile(hFile, &OBBDesc, sizeof(CBounding_OBB::BOUNDINGOBBDESC), &dwByte, nullptr);
		m_pColliderCom->Set_BoundingDesc(&OBBDesc);
		break;
	default:
		return E_FAIL;
	}

	CloseHandle(hFile);

	return S_OK;
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	Safe_Release(m_pBlockEffect);

	Safe_Release(m_pWire);
	Safe_Release(m_pSurge);
	Safe_Release(m_pShuriken);
	Safe_Release(m_pKatana);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShadowShaderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVisionColliderCom);
	Safe_Release(m_pBlinkColliderCom);
	Safe_Release(m_pBlockColliderCom);
	Safe_Release(m_pPlayerCameraCom);
}