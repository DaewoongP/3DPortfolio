#include "..\Public\Player.h"
#include "GameInstance.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CGameObject(rhs)
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
	m_pModelCom->Set_AnimIndex(0);

	m_fMouseSensitivity = 0.1f;

	return S_OK;
}

void CPlayer::Tick(_double dTimeDelta)
{
	Key_Input(dTimeDelta);

	__super::Tick(dTimeDelta);

	m_pModelCom->Play_Animation(dTimeDelta);

	Fix_Mouse();
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
	TransformDesc.dSpeedPerSec = 10.f;
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
	if (FAILED(m_pModelCom->Find_BoneIndex(TEXT("RootNode"), &m_iHeadChannelIndex)))
		return E_FAIL;

	return S_OK;
}

void CPlayer::Key_Input(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKeyState(DIK_W))
	{
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

	_long		dwMouseMove = 0;
	CameraOffset();

	if (dwMouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMM_X))
	{
		_vector	vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

		m_pTransformCom->Turn(vUp, dwMouseMove * dTimeDelta * m_fMouseSensitivity);
		m_pPlayerCameraCom->Turn(vUp, dwMouseMove * dTimeDelta * m_fMouseSensitivity);
	}

	if (dwMouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMM_Y))
	{
		// 회전축을 플레이어의 카메라 right 벡터 기준으로 처리.
		_vector	vCamRight = m_pPlayerCameraCom->Get_TransformState(CTransform::STATE_RIGHT);

		m_pPlayerCameraCom->Turn(vCamRight, dwMouseMove * dTimeDelta * m_fMouseSensitivity);
	}

	Safe_Release(pGameInstance);
}

void CPlayer::Fix_Mouse()
{
	POINT	ptMouse{ g_iWinSizeX >> 1, g_iWinSizeY >> 1 };

	ClientToScreen(g_hWnd, &ptMouse);
	SetCursorPos(ptMouse.x, ptMouse.y);
}

void CPlayer::CameraOffset()
{
	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPosition += XMVectorSet(0.f, 0.5f, 0.f, 0.f);

	m_pPlayerCameraCom->Set_Position(vPosition);
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
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pPlayerCameraCom);
}
