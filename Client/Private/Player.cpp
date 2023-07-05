#include "..\Public\Player.h"
#include "GameInstance.h"
#include "Part.h"
#include "Layer.h"
#include "Katana.h"
#include "ColProp.h"

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
	m_fSpeed = 1.5f;

	CTransform::TRANSFORMDESC TransformDesc = CTransform::TRANSFORMDESC(m_fSpeed, XMConvertToRadians(3.f));

	if (FAILED(__super::Initialize(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	if (FAILED(Add_Parts()))
		return E_FAIL;

	if (FAILED(Initailize_Skills()))
		return E_FAIL;
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	// 카메라 초기 값을 객체의 트랜스폼 월드값으로 초기화.
	m_pPlayerCameraCom->Set_CameraWorldMatrix(XMLoadFloat4x4(m_pTransformCom->Get_WorldFloat4x4()));
	// 모델의 애니메이션 인덱스 설정
	m_pModelCom->Set_AnimIndex(95);
	m_eCurState = STATE_IDLE;

	m_fMouseSensitivity = 0.2f;

	m_pTransformCom->Use_RigidBody(m_pNavigationCom);

	XMStoreFloat4x4(&m_CameraMatrix, XMMatrixIdentity());

	return S_OK;
}

// Tick : 이동 관련 연산 처리.
void CPlayer::Tick(_double dTimeDelta)
{
	Key_Input(dTimeDelta);
	Fix_Mouse();

	AnimationState(dTimeDelta);
	Attack();

	m_pTransformCom->Crouch(m_isCrouch, dTimeDelta, 2.f);

	Tick_Skills(dTimeDelta);

	WallRunCameraReset(dTimeDelta);

	__super::Tick(dTimeDelta);
	// 카메라 포지션 고정, 카메라 회전처리 이후 포지션 변경
	CameraOffset(dTimeDelta);
	// 충돌처리
	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
}

GAMEEVENT CPlayer::Late_Tick(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	//pGameInstance->Add_Collider(CCollision_Manager::COLTYPE_DYNAMIC, m_pColliderCom);

	// 아래와 같은 형태로 처리가능.
	//GAMEEVENT eGameEventFlag = __super::Late_Tick(dTimeDelta);
	__super::Late_Tick(dTimeDelta);

	// Y값이 특정 값 이하로 떨어지면 스테이지 초기화.
	if (XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) < -10.f)
	{
		// 사망처리도 여기서하면 좋을듯.
		Safe_Release(pGameInstance);
		return GAME_STAGE_RESET;
	}

	if (pGameInstance->Get_DIKeyState(DIK_R))
	{
		Safe_Release(pGameInstance);
		return GAME_STAGE_RESET;
	}

	Safe_Release(pGameInstance);

	return GAME_NOEVENT;
}

void CPlayer::OnCollisionEnter(COLLISIONDESC CollisionDesc)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// Wall Run
	if (!lstrcmp(static_cast<CGameObject*>(CollisionDesc.pOtherCollider->Get_Owner())->Get_LayerTag(), TEXT("Layer_ColProps")))
	{
		m_fWallRunY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

		if (m_fWallRunY > 4.f)
		{
			m_fWallRunAngle = 0.f;
			m_isWallRun = { true };
		}
		else
		{
			m_fWallRunY = 0.f;
		}
	}

	Safe_Release(pGameInstance);
}

void CPlayer::OnCollisionStay(COLLISIONDESC CollisionDesc)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (nullptr != pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Props")))
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
	if (m_fWallRunY > 4.f)
	{
		m_isWallRun = { false };
		m_fWallRunY = 0.f;
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
		// m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TextureType_NORMALS);

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

#ifdef _DEBUG
	m_pColliderCom->Render();
	m_pNavigationCom->Render();
#endif // _DEBUG

	return S_OK;
}

HRESULT CPlayer::Reset()
{
	m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());
	// 카메라 초기 값을 객체의 트랜스폼 월드값으로 초기화.
	m_pPlayerCameraCom->Set_CameraWorldMatrix(XMLoadFloat4x4(m_pTransformCom->Get_WorldFloat4x4()));
	// 모델의 애니메이션 인덱스 설정
	m_pModelCom->Set_AnimIndex(95);
	m_eCurState = STATE_IDLE;

	// 컴포넌트 리셋들 모두 호출해주려면 부모 불러줘야함.
	if (FAILED(__super::Reset()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Add_Component()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		MSG_BOX("Failed CPlayer Add_Component : (Com_Model)");
		return E_FAIL;
	}

	// Notify SetUp
	if (FAILED(SetUp_AnimationNotifies(TEXT("../../Resources/GameData/Notify/Att_R2.Notify"))))
		return E_FAIL;
	if (FAILED(SetUp_AnimationNotifies(TEXT("../../Resources/GameData/Notify/Run.Notify"))))
		return E_FAIL;
	if (FAILED(SetUp_AnimationNotifies(TEXT("../../Resources/GameData/Notify/WallRun_L.Notify"))))
		return E_FAIL;
	if (FAILED(SetUp_AnimationNotifies(TEXT("../../Resources/GameData/Notify/WallRun_R.Notify"))))
		return E_FAIL; 
	if (FAILED(SetUp_AnimationNotifies(TEXT("../../Resources/GameData/Notify/Hook_Pull.Notify"))))
		return E_FAIL;

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

	CNavigation::NAVIGATIONDESC NavigationDesc;
	NavigationDesc.iCurrentIndex = 0;

	/* For.Com_Navigation */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NavigationDesc)))
	{
		MSG_BOX("Failed CPlayer Add_Component : (Com_Navigation)");
		return E_FAIL;
	}

	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom))))
	{
		MSG_BOX("Failed CPlayer Add_Component : (Com_Collider)");
		return E_FAIL;
	}

	if (FAILED(SetUp_Collider(TEXT("../../Resources/GameData/Collider/Player.Col"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Add_Parts()
{
	CPart::PARENTMATRIXDESC ParentMatrixDesc;
	ZEROMEM(&ParentMatrixDesc);

	const CBone* pBone = m_pModelCom->Get_Bone(TEXT("Weapon_r"));

	ParentMatrixDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	ParentMatrixDesc.OffsetMatrix = pBone->Get_OffsetMatrix();
	ParentMatrixDesc.pCombindTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
	ParentMatrixDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();

	if (FAILED(__super::Add_Part(TEXT("Prototype_GameObject_Katana"), TEXT("Layer_PlayerWeapon"),
		TEXT("Part_Katana"), reinterpret_cast<CGameObject**>(&m_pKatana), &ParentMatrixDesc)))
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

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CPlayer::Initailize_Skills()
{
	/* Dash */
	m_Dash.isUsed = false;
	m_Dash.dCurTime = 0.0;
	m_Dash.dOriginCoolTime = 2.0;
	m_Dash.dCoolTime = 0.0;
	m_Dash.dDuration = 0.07;
	m_Dash.fLimitVelocity = 10.f;
	m_Dash.fSpeed = 50.f;

	return S_OK;
}

void CPlayer::Key_Input(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

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
		if (STATE_ATTACK != m_eCurState)
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
		if (STATE_ATTACK != m_eCurState)
			m_eCurState = STATE_IDLE;
	}
	// left
	if (pGameInstance->Get_DIKeyState(DIK_A))
	{
		m_pTransformCom->Go_Left(dTimeDelta);
	}
	// right
	if (pGameInstance->Get_DIKeyState(DIK_D))
	{
		m_pTransformCom->Go_Right(dTimeDelta);
	}
	// jump, SPACE
	if (false == m_pTransformCom->IsJumping() && pGameInstance->Get_DIKeyState(DIK_SPACE, CInput_Device::KEY_DOWN))
	{
		m_pTransformCom->Jump(6.f, dTimeDelta);
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
		if (STATE_ATTACK != m_eCurState)
			m_eCurState = STATE_IDLE;
		m_isCrouch = false;
	}
	// Hook
	if (STATE_HOOK != m_eCurState && pGameInstance->Get_DIMouseState(CInput_Device::DIMK_RBUTTON, CInput_Device::KEY_DOWN))
	{
		m_eCurState = STATE_HOOK;
		Check_Hook();
	}
	// attack, Lbutton, Lclick
	if (pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_DOWN))
	{
		if (STATE_ATTACK != m_eCurState &&
			STATE_CLIMB != m_eCurState &&
			STATE_DRONRIDE != m_eCurState)
		{
			m_eCurState = STATE_ATTACK;
		}

		if (STATE_ATTACK == m_eCurState &&
			ANIM_LERP != m_eCurrentAnimationFlag)
		{
			// 실제 게임이랑 속도 맞춤.
			// 40퍼센트가 넘으면 다음 공격 실행가능 (연속공격 처리.)
			if (0.4f <= m_pModelCom->Get_CurrentFramePercent())
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
#endif // _DEBUG
	// Mouse Move Horizontal
	if (dwMouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMM_X))
	{
		_vector	vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

		m_pTransformCom->Turn(vUp, dwMouseMove * m_fMouseSensitivity, dTimeDelta);
	}
	// Mouse Move Vertical
	if (dwMouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMM_Y))
	{
		// 회전축을 플레이어의 카메라 right 벡터 기준으로 처리.
		_vector	vCamRight = m_pPlayerCameraCom->Get_TransformState(CTransform::STATE_RIGHT);

		m_pTransformCom->Turn(vCamRight, dwMouseMove * m_fMouseSensitivity, dTimeDelta);
	}

	Safe_Release(pGameInstance);
}

void CPlayer::Fix_Mouse()
{
#ifdef _DEBUG
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKeyState(DIK_Q, CInput_Device::KEY_DOWN))
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

	m_pModelCom->Play_Animation(dTimeDelta, true, m_dAnimationLerpDuration);
}

void CPlayer::Motion_Change(ANIMATIONFLAG eAnimationFlag)
{
	// 모션 변경 처리 전 예외처리.
	if (false == (ANIM_PLAYING == eAnimationFlag || ANIM_FINISHED == eAnimationFlag))
		return;

	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case STATE_IDLE:
			m_pModelCom->Set_AnimIndex(95);
			break;
		case STATE_ATTACK:
			m_pModelCom->Set_AnimIndex(84 + rand() % 6, false);
			break;
		case STATE_RUN:
			m_pModelCom->Set_AnimIndex(102);
			break;
		case STATE_RUNWALL_L:
			m_pModelCom->Set_AnimIndex(103);
			break;
		case STATE_RUNWALL_R:
			m_pModelCom->Set_AnimIndex(104);
			break;
		case STATE_CROUCH:
			m_pModelCom->Set_AnimIndex(92);
			break;
		case STATE_HOOK:
			m_pModelCom->Set_AnimIndex(62, false);
			break;
		case STATE_CLIMB:
			break;
		case STATE_DRONRIDE:
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

void CPlayer::Attack()
{
	if (STATE_ATTACK != m_eCurState)
		return;

	if (0.15f <= m_pModelCom->Get_CurrentFramePercent() && 0.8 > m_pModelCom->Get_CurrentFramePercent())
		m_pKatana->Attack();
}

void CPlayer::CollisionStayWall(COLLISIONDESC CollisionDesc)
{
	CGameObject* pWall = static_cast<CGameObject*>(CollisionDesc.pOtherCollider->Get_Owner());

	_vector vWallPos = pWall->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	// Right 기본이 -z
	XMStoreFloat3(&m_vWallDir, XMVector3Normalize(pWall->Get_Transform()->Get_State(CTransform::STATE_RIGHT)));
	// Look 기본이 +x
	_vector vWallLook = XMVector3Normalize(pWall->Get_Transform()->Get_State(CTransform::STATE_LOOK));
	_vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos.m128_f32[1] = 0.f;
	vWallPos.m128_f32[1] = 0.f;

	_vector vDir = vWallPos - vPos;

	if (0 < XMVectorGetX(XMVector3Dot(XMVector3Normalize(vDir), XMVector3Normalize(vWallLook))))
	{
		if (-15.f < m_fWallRunAngle)
			m_fWallRunAngle -= 60.f * (_float)g_TimeDelta;
		
		if (0 < XMVectorGetX(XMVector3Dot(vLook, XMLoadFloat3(&m_vWallDir))))
			m_eCurState = STATE_RUNWALL_L;
		else
			m_eCurState = STATE_RUNWALL_R;
		// Turn
		m_pPlayerCameraCom->Rotation(XMLoadFloat3(&m_vWallDir), XMConvertToRadians(m_fWallRunAngle), true);
	}
	else
	{
		if (15.f > m_fWallRunAngle)
			m_fWallRunAngle += 60.f * (_float)g_TimeDelta;

		if (0 < XMVectorGetX(XMVector3Dot(vLook, XMLoadFloat3(&m_vWallDir))))
			m_eCurState = STATE_RUNWALL_R;
		else
			m_eCurState = STATE_RUNWALL_L;

		m_pPlayerCameraCom->Rotation(XMLoadFloat3(&m_vWallDir), XMConvertToRadians(m_fWallRunAngle), true);
	}

	if (0 < XMVectorGetX(XMVector3Dot(vLook, XMLoadFloat3(&m_vWallDir))))
		m_pTransformCom->WallRun(m_fWallRunY, XMLoadFloat3(&m_vWallDir) * 0.3f);
	else
		m_pTransformCom->WallRun(m_fWallRunY, XMLoadFloat3(&m_vWallDir) * -0.3f);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	
	// 벽점프
	if (pGameInstance->Get_DIKeyState(DIK_SPACE, CInput_Device::KEY_DOWN))
	{
		m_fWallRunY = 0.f;
		m_isWallRun = false;
		if (0 > m_fWallRunAngle)
		{
			m_pTransformCom->Jump(-vWallLook + vLook * 0.7f + XMVectorSet(0.f, 0.5f, 0.f, 0.f), 4.f, g_TimeDelta);
		}
		else
		{
			m_pTransformCom->Jump(vWallLook + vLook * 0.7f + XMVectorSet(0.f, 0.5f, 0.f, 0.f), 4.f, g_TimeDelta);
		}
	}

	Safe_Release(pGameInstance);
}

_bool CPlayer::Check_Hook()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	_float4 vRayPos, vRayDir;

	if (FAILED(pGameInstance->Get_WorldMouseRay(m_pContext, g_hWnd, &vRayPos, &vRayDir)))
	{
		Safe_Release(pGameInstance);
		return false;
	}
	Safe_Release(pGameInstance);
	CLayer* pHookLayer = pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Hook"));
	// 거리별 컬링은 나중에 생각하고.
	_float fDist = 100.f;

	for (auto& pObject : pHookLayer->Get_AllGameObject())
	{
		CCollider* pCollider = static_cast<CColProp*>(pObject.second)->Get_Collider();
		if (nullptr == pCollider)
			continue;

		if (true == pCollider->RayIntersects(XMLoadFloat4(&vRayPos), XMLoadFloat4(&vRayDir), fDist))
		{
			if (100.f > fDist)
			{
				m_pTransformCom->Jump(XMVector4Normalize(XMLoadFloat4(&vRayDir)), fDist * 0.5f, g_TimeDelta);
				return true;
			}
		}
	}

	return false;
}

void CPlayer::Tick_Skills(_double dTimeDelta)
{
	Dash(dTimeDelta);
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
		// shift 미리눌림방지
		m_Dash.isUsed = false;
	}
	// 누르질 않음.
	if (false == m_Dash.isUsed)
		return;

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

	Safe_Release(m_pKatana);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pPlayerCameraCom);
}
