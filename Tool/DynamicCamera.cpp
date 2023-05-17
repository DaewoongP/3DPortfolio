#include "pch.h"
#include "DynamicCamera.h"

CDynamicCamera::CDynamicCamera(LPDIRECT3DDEVICE9 pDevice)
	:CGameObject(pDevice)
	, m_bFix(true)
	, m_bClick(false)
{
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
	, m_fSpeed(rhs.m_fSpeed)
	, m_bFix(rhs.m_bFix)
	, m_bClick(rhs.m_bClick)
{
}

HRESULT CDynamicCamera::Initialize_Prototype()
{
	m_vEye = _vec3(0.f, 0.f, -1.f);
	m_vAt = _vec3(0.f, 0.f, 0.f);
	m_vUp = _vec3(0.f, 1.f, 0.f);
	m_fFov = D3DXToRadian(60.f);
	m_fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	m_fNear = 0.1f;
	m_fFar = 1000.f;
	m_fSpeed = 50.f;
	return S_OK;
}

HRESULT CDynamicCamera::Initialize(void* pArg)
{
	return S_OK;
}

void CDynamicCamera::Tick(_double dTimeDelta)
{
	Key_Input(dTimeDelta);

	/*if (m_bFix)
	{
		Fix_Mouse();
		Mouse_Move();
	}*/

	D3DXMatrixPerspectiveFovLH(&m_matProj, m_fFov, m_fAspect, m_fNear, m_fFar);
	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);

	m_pDevice->SetTransform(D3DTS_VIEW, &m_matView);
	m_pDevice->SetTransform(D3DTS_PROJECTION, &m_matProj);
}

HRESULT CDynamicCamera::Add_Components()
{
	return S_OK;
}

void CDynamicCamera::Key_Input(const _double& dTimeDelta)
{
	_matrix		matCamWorld;
	D3DXMatrixInverse(&matCamWorld, 0, &m_matView);

	if (GetAsyncKeyState('W') & 0x8000)
	{
		_vec3		vLook;
		memcpy(&vLook, &matCamWorld.m[2][0], sizeof(_vec3));

		_vec3	vLength = *D3DXVec3Normalize(&vLook, &vLook) * dTimeDelta * m_fSpeed;

		m_vEye += vLength;
		m_vAt += vLength;
	}

	if (GetAsyncKeyState('S') & 0x8000)
	{
		_vec3		vLook;
		memcpy(&vLook, &matCamWorld.m[2][0], sizeof(_vec3));

		_vec3	vLength = *D3DXVec3Normalize(&vLook, &vLook) * dTimeDelta * m_fSpeed;

		m_vEye -= vLength;
		m_vAt -= vLength;
	}

	if (GetAsyncKeyState('A') & 0x8000)
	{
		_vec3		vRight;
		memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_vec3));

		_vec3	vLength = *D3DXVec3Normalize(&vRight, &vRight) * dTimeDelta * m_fSpeed;

		m_vEye -= vLength;
		m_vAt -= vLength;
	}

	if (GetAsyncKeyState('D') & 0x8000)
	{
		_vec3		vRight;
		memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_vec3));

		_vec3	vLength = *D3DXVec3Normalize(&vRight, &vRight) * dTimeDelta * m_fSpeed;

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
	/*_long		dwMouseMove = 0;

	_matrix		matCamWorld;
	D3DXMatrixInverse(&matCamWorld, 0, &m_matView);

	if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_Y))
	{
		_vec3	vRight;
		memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_vec3));

		_matrix		matRot;
		D3DXMatrixRotationAxis(&matRot, &vRight, D3DXToRadian(dwMouseMove / 10.f));

		_vec3	vLook = m_vAt - m_vEye;
		D3DXVec3TransformNormal(&vLook, &vLook, &matRot);

		m_vAt = m_vEye + vLook;
	}

	if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_X))
	{
		_vec3	vUp;
		memcpy(&vUp, &matCamWorld.m[1][0], sizeof(_vec3));

		_matrix		matRot;
		D3DXMatrixRotationAxis(&matRot, &vUp, D3DXToRadian(dwMouseMove / 10.f));

		_vec3	vLook = m_vAt - m_vEye;
		D3DXVec3TransformNormal(&vLook, &vLook, &matRot);

		m_vAt = m_vEye + vLook;
	}*/
}

void CDynamicCamera::Fix_Mouse(void)
{
	POINT	ptMouse{ g_iWinSizeX >> 1, g_iWinSizeY >> 1 };

	ClientToScreen(g_hWnd, &ptMouse);
	SetCursorPos(ptMouse.x, ptMouse.y);
}

CDynamicCamera* CDynamicCamera::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CDynamicCamera* pInstance = new CDynamicCamera(pDevice);

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
}
