#include "..\Public\Player.h"
#include "GameInstance.h"

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
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	// 카메라 초기 값을 객체의 트랜스폼 월드값으로 초기화.
	m_pPlayerCameraCom->Set_CameraWorldMatrix(XMLoadFloat4x4(m_pTransformCom->Get_WorldFloat4x4()));
	// 모델의 애니메이션 인덱스 설정
	m_pModelCom->Set_AnimIndex(95);
	m_eCurState = STATE_IDLE;

	m_fMouseSensitivity = 0.1f;

	// 카메라 초기 설정.
	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	vPosition += vLook * 0.5f + XMVectorSet(0.f, 4.f, 0.f, 0.f);

	m_pPlayerCameraCom->Set_Position(vPosition);

	return S_OK;
}

void CPlayer::Tick(_double dTimeDelta)
{
	// 애니메이션이 종료 되었을때 따로 처리되는 명령이 없을 경우 IDLE처리
	if (ANIM_FINISHED == m_eCurrentAnimationFlag)
		m_eCurState = STATE_IDLE;

	// Input이 Tick보다 위에 있어야 걸리는게 없어짐.
	Key_Input(dTimeDelta);

	Fix_Mouse();

	__super::Tick(dTimeDelta);

	m_eCurrentAnimationFlag = m_pModelCom->Play_Animation(dTimeDelta);

	Motion_Change(m_eCurrentAnimationFlag);
	
	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
}

void CPlayer::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
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
#endif // _DEBUG

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

	// Get Model's Bone Index
	if (FAILED(Find_BoneIndices()))
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

	CTransform::TRANSFORMDESC TransformDesc;
	TransformDesc.dSpeedPerSec = 15.f;
	TransformDesc.dRotationPerSec = 3.f;

	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
	{
		MSG_BOX("Failed CPlayer Add_Component : (Com_Transform)");
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
	CameraDesc.TransformDesc.dSpeedPerSec = TransformDesc.dSpeedPerSec;
	CameraDesc.TransformDesc.dRotationPerSec = TransformDesc.dRotationPerSec;

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
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigation), &NavigationDesc)))
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

	if (FAILED(SetUp_Collider(TEXT("../../Resources/GameData/Collider/Collider.Col"))))
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

HRESULT CPlayer::Find_BoneIndices()
{
	if (FAILED(m_pModelCom->Find_BoneIndex(TEXT("Weapon_r"), &m_iWeaponR)))
		return E_FAIL;

	return S_OK;
}

void CPlayer::Key_Input(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKeyState(DIK_W))
	{
		if (STATE_IDLE == m_eCurState)
			m_eCurState = STATE_RUN;
		m_pTransformCom->Go_Straight(dTimeDelta, m_pNavigation);
	}

	if (pGameInstance->Get_DIKeyState(DIK_S))
	{
		m_pTransformCom->Go_Backward(dTimeDelta, m_pNavigation);
	}

	if (pGameInstance->Get_DIKeyState(DIK_A))
	{
		m_pTransformCom->Go_Left(dTimeDelta, m_pNavigation);
	}

	if (pGameInstance->Get_DIKeyState(DIK_D))
	{
		m_pTransformCom->Go_Right(dTimeDelta, m_pNavigation);
	}

	if (pGameInstance->Get_DIKeyState(DIK_SPACE))
	{
		//m_pTransformCom->Jump(dTimeDelta);
	}

	if (pGameInstance->Get_DIKeyState(DIK_C))
	{
		// crouch
	}

	if (pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_DOWN))
	{
		m_eCurState = STATE_ATTACK;
	}

	_long		dwMouseMove = 0;

	CameraOffset();

#ifdef _DEBUG
	if (false == m_isMouseFixed)
	{
		Safe_Release(pGameInstance);
		return;
	}
#endif // _DEBUG

	if (dwMouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMM_X))
	{
		_vector	vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

		m_pTransformCom->Turn(vUp, dwMouseMove * m_fMouseSensitivity, dTimeDelta);
		m_pPlayerCameraCom->Turn(vUp, dwMouseMove * m_fMouseSensitivity, dTimeDelta);
	}

	if (dwMouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMM_Y))
	{
		// 회전축을 플레이어의 카메라 right 벡터 기준으로 처리.
		_vector	vCamRight = m_pPlayerCameraCom->Get_TransformState(CTransform::STATE_RIGHT);

		m_pPlayerCameraCom->Turn(vCamRight, dwMouseMove * m_fMouseSensitivity, dTimeDelta);
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

void CPlayer::CameraOffset()
{
	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	vPosition += vLook * 0.5f + XMVectorSet(0.f, 4.f, 0.f, 0.f);

	m_pPlayerCameraCom->Set_Position(vPosition);
}

void CPlayer::Motion_Change(ANIMATIONFLAG eAnimationFlag)
{
	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case Client::CPlayer::STATE_IDLE:
			m_pModelCom->Set_AnimIndex(95);
			break;
		case Client::CPlayer::STATE_ATTACK:
			m_pModelCom->Set_AnimIndex(84 + rand() % 6, false);
			break;
		case Client::CPlayer::STATE_RUN:
			m_pModelCom->Set_AnimIndex(102, false);
			break;
		case Client::CPlayer::STATE_RUNWALL:
			break;
		case Client::CPlayer::STATE_CROUCH:
			break;
		case Client::CPlayer::STATE_JUMP:
			break;
		case Client::CPlayer::STATE_CLIMB:
			break;
		case Client::CPlayer::STATE_DRONRIDE:
			break;
		}
		m_ePreState = m_eCurState;
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
	
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pNavigation);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pPlayerCameraCom);
}
