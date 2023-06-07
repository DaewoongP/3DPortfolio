#include "Camera_Free.h"
#include "GameInstance.h"

CCamera_Free::CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
	, m_bFix(true)
	, m_bClick(false)
{
}

CCamera_Free::CCamera_Free(const CCamera_Free& rhs)
	: CGameObject(rhs)
	, m_bFix(rhs.m_bFix)
	, m_bClick(rhs.m_bClick)
{
}

void CCamera_Free::Set_Speed(_float fSpeed)
{
	m_pCamera->Set_TransformDesc(CTransform::TRANSFORMDESC(fSpeed, 3.f));
}

HRESULT CCamera_Free::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CCamera_Free::Initialize(void* pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(pArg), E_FAIL);
	FAILED_CHECK_RETURN(Add_Components(), E_FAIL);

	return S_OK;
}

void CCamera_Free::Tick(_double dTimeDelta)
{
	Key_Input(dTimeDelta);

	if (m_bFix)
	{
		Fix_Mouse();
		Mouse_Move(dTimeDelta);
	}

	__super::Tick(dTimeDelta);
}

HRESULT CCamera_Free::Add_Components()
{
	CCamera::CAMERADESC CameraDesc;
	CameraDesc.vEye = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 1.f, 1.f);
	CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);

	CameraDesc.fFovy = XMConvertToRadians(90.f);
	CameraDesc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 1000.f;
	CameraDesc.TransformDesc.dSpeedPerSec = 5.f;
	CameraDesc.TransformDesc.dRotationPerSec = 3.f;

	if (FAILED(__super::Add_Component(LEVEL_TOOL,
		TEXT("Prototype_Component_Camera"),
		TEXT("Com_Camera"), reinterpret_cast<CComponent**>(&m_pCamera), &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

void CCamera_Free::Key_Input(const _double& dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (pGameInstance->Get_DIKeyState(DIK_W))
	{
		m_pCamera->Go_Straight(dTimeDelta);
	}

	if (pGameInstance->Get_DIKeyState(DIK_S))
	{
		m_pCamera->Go_Backward(dTimeDelta);
	}

	if (pGameInstance->Get_DIKeyState(DIK_A))
	{
		m_pCamera->Go_Left(dTimeDelta);
	}

	if (pGameInstance->Get_DIKeyState(DIK_D))
	{
		m_pCamera->Go_Right(dTimeDelta);
	}

	if (pGameInstance->Get_DIKeyState(DIK_Q, CInput_Device::KEY_DOWN))
	{
		if (m_bClick)
			return;

		m_bClick = true;

		if (true == m_bFix)
			m_bFix = false;
		else
			m_bFix = true;
	}
	else
		m_bClick = false;

	if (false == m_bFix)
		return;

}

void CCamera_Free::Mouse_Move(_double dTimeDelta)
{
	_long		dwMouseMove = 0;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (dwMouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMM_X))
	{
		_vector	vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

		m_pCamera->Turn(vUp, dwMouseMove * dTimeDelta * 0.1f);
	}

	if (dwMouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMM_Y))
	{
		_vector	vRight = m_pCamera->Get_TransformState(CTransform::STATE_RIGHT);

		m_pCamera->Turn(vRight, dwMouseMove * dTimeDelta * 0.1f);
	}

}

void CCamera_Free::Fix_Mouse(void)
{
	POINT	ptMouse{ g_iWinSizeX >> 1, g_iWinSizeY >> 1 };

	ClientToScreen(g_hWnd, &ptMouse);
	SetCursorPos(ptMouse.x, ptMouse.y);
}

_vector CCamera_Free::Picking()
{
	POINT	pt{};
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	_vector		vMouse;
	vMouse.m128_f32[0] = pt.x / (g_iWinSizeX * 0.5f) - 1.f;
	vMouse.m128_f32[1] = pt.y / -(g_iWinSizeY * 0.5f) + 1.f;
	vMouse.m128_f32[2] = 0.f;
	vMouse.m128_f32[3] = 1.f;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	_matrix		ProjMatrix_Inverse;
	ProjMatrix_Inverse = pGameInstance->Get_TransformMatrix_Inverse(CPipeLine::D3DTRANSFORMSTATE::D3DTS_PROJ);
	vMouse = XMVector3TransformCoord(vMouse, ProjMatrix_Inverse);

	_matrix		ViewMatrix_Inverse;
	ViewMatrix_Inverse = pGameInstance->Get_TransformMatrix_Inverse(CPipeLine::D3DTRANSFORMSTATE::D3DTS_VIEW);

	_vector vRayPos, vRayDir;
	vRayPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	vRayDir = vMouse - vRayPos;
	vRayPos = XMVector3TransformCoord(vRayPos, ViewMatrix_Inverse);
	vRayDir = XMVector3TransformNormal(vRayDir, ViewMatrix_Inverse);

	/*vRayDir.m128_f32[0] /= vRayDir.m128_f32[2];
	vRayDir.m128_f32[1] /= vRayDir.m128_f32[2];
	vRayDir.m128_f32[2] /= vRayDir.m128_f32[2];
	vRayDir.m128_f32[3] = 0.f;*/
	_float fCamToPickingDistance = { 30.f };
	_vector vPos = vRayPos + vRayDir * fCamToPickingDistance;

	return vPos;
}

CCamera_Free* CCamera_Free::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Free* pInstance = New CCamera_Free(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CCamera_Free");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCamera_Free::Clone(void* pArg)
{
	CCamera_Free* pInstance = New CCamera_Free(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CCamera_Free");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCamera_Free::Free()
{
	__super::Free();
	Safe_Release(m_pCamera);
}
