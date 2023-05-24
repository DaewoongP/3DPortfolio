#include "pch.h"
#include "DynamicCamera.h"
#include "GameInstance.h"

CDynamicCamera::CDynamicCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
	, m_bFix(true)
	, m_bClick(false)
	, m_pGameInstance{ CGameInstance::GetInstance() }
{
	m_matView = XMMatrixIdentity();
	m_matProj = XMMatrixIdentity();
	m_matCam = XMMatrixIdentity();
}

CDynamicCamera::CDynamicCamera(const CDynamicCamera& rhs)
	:CGameObject(rhs)
	, m_vEye(rhs.m_vEye)
	, m_vAt(rhs.m_vAt)
	, m_vUp(rhs.m_vUp)
	, m_fFov(rhs.m_fFov)
	, m_fAspect(rhs.m_fAspect)
	, m_fNear(rhs.m_fNear)
	, m_fFar(rhs.m_fFar)
	, m_matView(rhs.m_matView)
	, m_matProj(rhs.m_matProj)
	, m_fSpeed(rhs.m_fSpeed)
	, m_bFix(rhs.m_bFix)
	, m_bClick(rhs.m_bClick)
	, m_matCam(rhs.m_matCam)
	, m_pGameInstance(rhs.m_pGameInstance)
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CDynamicCamera::Initialize_Prototype()
{
	m_vEye = _vector{ 0.f, 0.f, -1.f };
	m_vAt = _vector{ 0.f, 0.f, 0.f };
	m_vUp = _vector{ 0.f, 1.f, 0.f };

	m_fFov = XMConvertToRadians(60.f);
	m_fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	m_fNear = 0.1f;
	m_fFar = 1000.f;
	m_fSpeed = 5.f;
	return S_OK;
}

HRESULT CDynamicCamera::Initialize(void* pArg)
{
	return S_OK;
}

void CDynamicCamera::Tick(_double dTimeDelta)
{
	Key_Input(dTimeDelta);

	if (m_bFix)
	{
		Fix_Mouse();
		Mouse_Move();
	}

	m_matView = XMMatrixLookAtLH(m_vEye, m_vAt, m_vUp);
	m_matProj = XMMatrixPerspectiveFovLH(m_fFov, m_fAspect, m_fNear, m_fFar);

	m_matCam = m_matView * m_matProj;
}

HRESULT CDynamicCamera::Add_Components()
{
	return S_OK;
}

void CDynamicCamera::Key_Input(const _double& dTimeDelta)
{
	_matrix		matCamWorld;
	matCamWorld = XMMatrixInverse(nullptr, m_matView);

	if (GetAsyncKeyState('W') & 0x8000)
	{
		_vector		vLook;
		memcpy(&vLook, &matCamWorld.r[2], sizeof(_vector));

		_vector	vLength = XMVector3Normalize(vLook) * static_cast<_float>(dTimeDelta) * m_fSpeed;
		
		m_vEye += vLength;
		m_vAt += vLength;
	}

	if (GetAsyncKeyState('S') & 0x8000)
	{
		_vector		vLook;
		memcpy(&vLook, &matCamWorld.r[2], sizeof(_vector));

		_vector	vLength = XMVector3Normalize(vLook) * static_cast<_float>(dTimeDelta) * m_fSpeed;

		m_vEye -= vLength;
		m_vAt -= vLength;
	}

	if (GetAsyncKeyState('A') & 0x8000)
	{
		_vector		vRight;
		memcpy(&vRight, &matCamWorld.r[0], sizeof(_vector));

		_vector	vLength = XMVector3Normalize(vRight) * static_cast<_float>(dTimeDelta) * m_fSpeed;

		m_vEye -= vLength;
		m_vAt -= vLength;
	}

	if (GetAsyncKeyState('D') & 0x8000)
	{
		_vector		vRight;
		memcpy(&vRight, &matCamWorld.r[0], sizeof(_vector));

		_vector	vLength = XMVector3Normalize(vRight) * static_cast<_float>(dTimeDelta) * m_fSpeed;

		m_vEye += vLength;
		m_vAt += vLength;
	}

	if (GetAsyncKeyState('Q') & 0x8000)
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

void CDynamicCamera::Mouse_Move(void)
{
	_long		dwMouseMove = 0;

	_matrix		matCamWorld;
	matCamWorld = XMMatrixInverse(nullptr, m_matView);
	
	if (dwMouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_Y))
	{
		_vector	vRight;
		memcpy(&vRight, &matCamWorld.r[0], sizeof(_vector));

		_matrix		matRot;
		matRot = XMMatrixRotationAxis(vRight, XMConvertToRadians(dwMouseMove / 10.f));

		_vector	vLook = m_vAt - m_vEye;
		vLook = XMVector3TransformNormal(vLook, matRot);

		m_vAt = m_vEye + vLook;
	}

	if (dwMouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_X))
	{
		_vector	vUp;
		memcpy(&vUp, &matCamWorld.r[1], sizeof(_vector));

		_matrix		matRot;
		matRot = XMMatrixRotationAxis(vUp, XMConvertToRadians(dwMouseMove / 10.f));

		_vector	vLook = m_vAt - m_vEye;
		vLook = XMVector3TransformNormal(vLook, matRot);
		
		m_vAt = m_vEye + vLook;
	}
}

void CDynamicCamera::Fix_Mouse(void)
{
	POINT	ptMouse{ g_iWinSizeX >> 1, g_iWinSizeY >> 1 };

	ClientToScreen(g_hViewWnd, &ptMouse);
	SetCursorPos(ptMouse.x, ptMouse.y);
}

_vector CDynamicCamera::Picking()
{
	POINT	pt{};
	GetCursorPos(&pt);
	ScreenToClient(g_hViewWnd, &pt);

	_vector		vMouse;
	vMouse.m128_f32[0] = pt.x / (g_iWinSizeX * 0.5f) - 1.f;
	vMouse.m128_f32[1] = pt.y / -(g_iWinSizeY * 0.5f) + 1.f;
	vMouse.m128_f32[2] = 0.f;
	vMouse.m128_f32[3] = 1.f;

	_matrix		matProj;
	matProj = XMMatrixInverse(nullptr, m_matProj);
	vMouse = XMVector3TransformCoord(vMouse, matProj);

	_matrix matView;
	matView = XMMatrixInverse(nullptr, m_matView);
	
	_vector vRayPos, vRayDir;
	vRayPos = { 0.f,0.f,0.f, 0.f};
	vRayDir = vMouse - vRayPos;
	vRayPos = XMVector3TransformCoord(vRayPos, matView);
	vRayDir = XMVector3TransformNormal(vRayDir, matView);
	
	vRayDir.m128_f32[0] /= vRayDir.m128_f32[1];
	vRayDir.m128_f32[1] /= vRayDir.m128_f32[1];	vRayDir.m128_f32[2] /= vRayDir.m128_f32[1];
	vRayDir.m128_f32[3] = 0.f;

	_vector vPos = vRayPos + vRayDir * (-vRayPos.m128_f32[1]);

	return vPos;
}

CDynamicCamera* CDynamicCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDynamicCamera* pInstance = new CDynamicCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CDynamicCamera");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CDynamicCamera::Clone(void* pArg)
{
	CDynamicCamera* pInstance = new CDynamicCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CDynamicCamera");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDynamicCamera::Free()
{
	__super::Free();
	Safe_Release(m_pGameInstance);
}
