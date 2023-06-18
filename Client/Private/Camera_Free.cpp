#include "..\Public\Camera_Free.h"
#include "GameInstance.h"

CCamera_Free::CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CCamera_Free::CCamera_Free(const CCamera_Free& rhs)
	: CGameObject(rhs)
	, m_bFix(rhs.m_bFix)
	, m_bClick(rhs.m_bClick)
{
}

HRESULT CCamera_Free::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CCamera_Free::Initialize(void* pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(pArg), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	return S_OK;
}

void CCamera_Free::Tick(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (pGameInstance->Get_DIKeyState(DIK_T, CInput_Device::KEY_PRESSING))
	{
		_vector vPos = m_pCameraCom->Get_TransformState(CTransform::STATE_POSITION);
		cout << "Current Cam Pos | ";
		cout << "X ( " << vPos.m128_f32[0] << " ) Y ( " << vPos.m128_f32[1] << " ) Z (" << vPos.m128_f32[2] << " )" << endl;
		__super::Tick(dTimeDelta);
		Key_Input(dTimeDelta);

		if (m_bFix)
		{
			Fix_Mouse();
			Mouse_Move(dTimeDelta);
		}
	}
}

void CCamera_Free::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);
}

HRESULT CCamera_Free::Add_Component()
{
	CCamera::CAMERADESC CameraDesc;
	CameraDesc.vEye = _float4(-2.f, 3.f, -2.f, 1.f);
	CameraDesc.vAt = _float4(5.f, 0.f, 5.f, 1.f);
	CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);
	
	CameraDesc.fFovy = XMConvertToRadians(60.f);
	CameraDesc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 1000.f;
	CameraDesc.TransformDesc.dSpeedPerSec = 5.f;
	CameraDesc.TransformDesc.dRotationPerSec = 3.f;

	/* For.Com_Camera */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Camera"),
		TEXT("Com_Camera"), reinterpret_cast<CComponent**>(&m_pCameraCom), &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

void CCamera_Free::Key_Input(const _double& dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (pGameInstance->Get_DIKeyState(DIK_W, CInput_Device::KEY_PRESSING))
	{
		m_pCameraCom->Go_Straight(dTimeDelta);
	}

	if (pGameInstance->Get_DIKeyState(DIK_S, CInput_Device::KEY_PRESSING))
	{
		m_pCameraCom->Go_Backward(dTimeDelta);
	}

	if (pGameInstance->Get_DIKeyState(DIK_A, CInput_Device::KEY_PRESSING))
	{
		m_pCameraCom->Go_Left(dTimeDelta);
	}

	if (pGameInstance->Get_DIKeyState(DIK_D, CInput_Device::KEY_PRESSING))
	{
		m_pCameraCom->Go_Right(dTimeDelta);
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
	Safe_AddRef(pGameInstance);

	if (dwMouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMM_X))
	{
		_vector	vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

		m_pCameraCom->Turn(vUp, dwMouseMove * dTimeDelta * 0.1f);
	}

	if (dwMouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMM_Y))
	{
		_vector	vRight = m_pCameraCom->Get_TransformState(CTransform::STATE_RIGHT);

		m_pCameraCom->Turn(vRight, dwMouseMove * dTimeDelta * 0.1f);
	}

	Safe_Release(pGameInstance);
}

void CCamera_Free::Fix_Mouse(void)
{
	POINT	ptMouse{ g_iWinSizeX >> 1, g_iWinSizeY >> 1 };

	ClientToScreen(g_hWnd, &ptMouse);
	SetCursorPos(ptMouse.x, ptMouse.y);
}


CCamera_Free* CCamera_Free::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Free* pInstance = new CCamera_Free(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CCamera_Free");
		Safe_Release(pInstance);
	}
	return pInstance;
}


CGameObject* CCamera_Free::Clone(void* pArg)
{
	CCamera_Free* pInstance = new CCamera_Free(*this);

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

	Safe_Release(m_pCameraCom);
}
