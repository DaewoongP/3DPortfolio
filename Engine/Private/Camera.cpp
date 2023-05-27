#include "..\Public\Camera.h"
#include "PipeLine.h"

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComposite(pDevice, pContext)
	, m_pPipeLine{ CPipeLine::GetInstance() }
{
	ZeroMemory(&m_vEye, sizeof m_vEye);
	ZeroMemory(&m_vAt, sizeof m_vAt);
	ZeroMemory(&m_vUp, sizeof m_vUp);

	Safe_AddRef(m_pPipeLine);
}

CCamera::CCamera(const CCamera& rhs)
	: CComposite(rhs)
	, m_vEye(rhs.m_vEye)
	, m_vAt(rhs.m_vAt)
	, m_vUp(rhs.m_vUp)
	, m_fFovy(rhs.m_fFovy)
	, m_fAspect(rhs.m_fAspect)
	, m_fNear(rhs.m_fNear)
	, m_fFar(rhs.m_fFar)
	, m_pPipeLine(rhs.m_pPipeLine)
{
	Safe_AddRef(m_pPipeLine);
}

HRESULT CCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera::Initialize(void* pArg)
{
	m_pTransform = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTransform)
		return E_FAIL;

	if (nullptr != pArg)
	{
		CAMERADESC	CameraDesc = *(CAMERADESC*)pArg;

		m_vEye = CameraDesc.vEye;
		m_vAt = CameraDesc.vAt;
		m_vUp = CameraDesc.vUp;

		m_fFovy = CameraDesc.fFovy;
		m_fAspect = CameraDesc.fAspect;
		m_fNear = CameraDesc.fNear;
		m_fFar = CameraDesc.fFar;

		m_pTransform->Set_Desc(CameraDesc.TransformDesc);
	}

	m_pTransform->Set_State(CTransform::STATE::STATE_POSITION, XMLoadFloat4(&m_vEye));
	m_pTransform->LookAt(XMLoadFloat4(&m_vAt));

	return S_OK;
}

void CCamera::Tick(_double dTimeDelta)
{
	m_pPipeLine->Set_Transform(CPipeLine::D3DTRANSFORMSTATE::D3DTS_VIEW, m_pTransform->Get_WorldMatrix_Inverse());
	m_pPipeLine->Set_Transform(CPipeLine::D3DTRANSFORMSTATE::D3DTS_PROJ, XMMatrixPerspectiveFovLH(m_fFovy, m_fAspect, m_fNear, m_fFar));
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
