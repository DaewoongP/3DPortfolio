#include "..\Public\Camera.h"

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjectionMatrix, XMMatrixIdentity());
	ZeroMemory(&m_CameraDesc, sizeof m_CameraDesc);
}

CCamera::CCamera(const CCamera& rhs)
	: CComponent(rhs)
	, m_ViewMatrix(rhs.m_ViewMatrix)
	, m_ProjectionMatrix(rhs.m_ProjectionMatrix)
	, m_CameraDesc(rhs.m_CameraDesc)
{
}

HRESULT CCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera::Initialize(void* pArg)
{
	if (nullptr != pArg)
		memmove(&m_CameraDesc, pArg, sizeof m_CameraDesc);

	return S_OK;
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
}
