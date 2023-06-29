#include "..\Public\Camera.h"
#include "PipeLine.h"

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComposite(pDevice, pContext)
	, m_pPipeLine{ CPipeLine::GetInstance() } 
	, m_fFovy(0.f)
	, m_fAspect(0.f)
	, m_fNear(0.f)
	, m_fFar(0.f)
{
	Safe_AddRef(m_pPipeLine);
	ZeroMemory(&m_vEye, sizeof m_vEye);
	ZeroMemory(&m_vAt, sizeof m_vAt);
	ZeroMemory(&m_vUp, sizeof m_vUp);
}

CCamera::CCamera(const CCamera& rhs)
	: CComposite(rhs)
	, m_pPipeLine(rhs.m_pPipeLine)
	, m_vEye(rhs.m_vEye)
	, m_vAt(rhs.m_vAt)
	, m_vUp(rhs.m_vUp)
	, m_fFovy(rhs.m_fFovy)
	, m_fAspect(rhs.m_fAspect)
	, m_fNear(rhs.m_fNear)
	, m_fFar(rhs.m_fFar)
{
	Safe_AddRef(m_pPipeLine);
	m_pTransform = static_cast<CTransform*>(rhs.m_pTransform->Clone(nullptr));
}

void CCamera::Set_CameraDesc(CAMERADESC CameraDesc)
{
	m_vEye = CameraDesc.vEye;
	m_vAt = CameraDesc.vAt;
	m_vUp = CameraDesc.vUp;

	m_fFovy = CameraDesc.fFovy;
	m_fAspect = CameraDesc.fAspect;
	m_fNear = CameraDesc.fNear;
	m_fFar = CameraDesc.fFar;

	m_pTransform->Set_State(CTransform::STATE::STATE_POSITION, XMLoadFloat4(&m_vEye));
	m_pTransform->LookAt(XMLoadFloat4(&m_vAt));
}

void CCamera::Set_LookAtLH(_float4 vEye, _float4 vAt, _float4 vUp)
{
	m_vEye = vEye;
	m_vAt = vAt;
	m_vUp = vUp;

	m_pTransform->Set_State(CTransform::STATE::STATE_POSITION, XMLoadFloat4(&vEye));
	m_pTransform->LookAt(XMLoadFloat4(&vAt));
}

HRESULT CCamera::Initialize_Prototype()
{
	m_pTransform = CTransform::Create(m_pDevice, m_pContext);

	if (nullptr == m_pTransform)
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		CAMERADESC CameraDesc = *static_cast<CAMERADESC*>(pArg);

		Set_CameraDesc(CameraDesc);

		m_pTransform->Set_Desc(CameraDesc.TransformDesc);
	}

	m_pTransform->Set_State(CTransform::STATE::STATE_POSITION, XMLoadFloat4(&m_vEye));
	m_pTransform->LookAt(XMLoadFloat4(&m_vAt));
	
	return S_OK;
}

GAMEEVENT CCamera::Late_Tick(_double dTimeDelta)
{
	NULL_CHECK_RETURN_MSG(m_pPipeLine, GAME_NOEVENT, TEXT("PipeLine NULL"));

	m_pPipeLine->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransform->Get_WorldMatrix_Inverse());
	m_pPipeLine->Set_Transform(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(m_fFovy, m_fAspect, m_fNear, m_fFar));

	return GAME_NOEVENT;
}

CCamera* CCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera* pInstance = new CCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CCamera");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CCamera::Clone(void* pArg)
{
	CCamera* pInstance = new CCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CCamera");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCamera::Free()
{
	__super::Free();

	Safe_Release(m_pPipeLine);
	Safe_Release(m_pTransform);
}
