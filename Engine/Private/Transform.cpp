#include "..\Public\Transform.h"
#include "Navigation.h"

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
	, m_TransformDesc(0.0, 0.0)
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
}

CTransform::CTransform(const CTransform& rhs)
	: CComponent(rhs)
	, m_TransformDesc(rhs.m_TransformDesc)
	, m_WorldMatrix(rhs.m_WorldMatrix)
{
}

_float3 CTransform::Get_Scale()
{
	return _float3(
		XMVectorGetX(XMVector3Length(Get_State(STATE::STATE_RIGHT))),
		XMVectorGetX(XMVector3Length(Get_State(STATE::STATE_UP))),
		XMVectorGetX(XMVector3Length(Get_State(STATE::STATE_LOOK)))
	);
}
void CTransform::Set_State(STATE _eState, _fvector _vState)
{
	_float4		vState;
	XMStoreFloat4(&vState, _vState);
	memcpy(&m_WorldMatrix.m[_eState][0], &vState, sizeof vState);
}

void CTransform::Set_Scale(const _float3& vScale)
{
	Set_State(STATE::STATE_RIGHT, XMVector3Normalize(Get_State(STATE::STATE_RIGHT)) * vScale.x);
	Set_State(STATE::STATE_UP, XMVector3Normalize(Get_State(STATE::STATE_UP)) * vScale.y);
	Set_State(STATE::STATE_LOOK, XMVector3Normalize(Get_State(STATE::STATE_LOOK)) * vScale.z);
}

HRESULT CTransform::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_TransformDesc, pArg, sizeof m_TransformDesc);

	return S_OK;
}

void CTransform::Move_Direction(_fvector vMoveDir, _double dTimeDelta, CNavigation* pNavigation)
{
	_vector vDir = XMVector3Normalize(vMoveDir);
	_vector vPosition = Get_State(STATE::STATE_POSITION);

	vPosition += vDir * static_cast<_float>(m_TransformDesc.dSpeedPerSec * dTimeDelta);
	
	_bool		isMove = true;
	_float3		vNormal;

	if (nullptr != pNavigation)
		isMove = pNavigation->Is_Move(vPosition, &vNormal);

	while (false == isMove)
	{
		vDir *= 0.99f;
		vPosition = Get_State(STATE::STATE_POSITION) + 
			(vDir - XMLoadFloat3(&vNormal) * (XMVector3Dot(vDir, XMLoadFloat3(&vNormal)))) * static_cast<_float>(m_TransformDesc.dSpeedPerSec * dTimeDelta);

		isMove = pNavigation->Is_Move(vPosition, &vNormal);
	}

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Straight(_double dTimeDelta, CNavigation* pNavigation)
{
	_vector vLook = Get_State(STATE::STATE_LOOK);
	Move_Direction(vLook, dTimeDelta, pNavigation);
}

void CTransform::Go_Backward(_double dTimeDelta, CNavigation* pNavigation)
{
	_vector vLook = Get_State(STATE::STATE_LOOK);
	Move_Direction(-vLook, dTimeDelta, pNavigation);
}

void CTransform::Go_Left(_double dTimeDelta, CNavigation* pNavigation)
{
	_vector vRight = Get_State(STATE::STATE_RIGHT);
	Move_Direction(-vRight, dTimeDelta, pNavigation);
}

void CTransform::Go_Right(_double dTimeDelta, CNavigation* pNavigation)
{
	_vector vRight = Get_State(STATE::STATE_RIGHT);
	Move_Direction(vRight, dTimeDelta, pNavigation);
}

void CTransform::Chase(_fvector vTargetPosition, _double dTimeDelta, _float fMinDistance)
{
	_vector vPosition = Get_State(STATE::STATE_POSITION);
	_vector vDir = vTargetPosition - vPosition;

	if (fMinDistance <= XMVectorGetX(XMVector3Length(vDir)))
		Move_Direction(vDir, dTimeDelta);
}

void CTransform::LookAt(_fvector vTargetPosition)
{
	_vector vPosition = Get_State(STATE::STATE_POSITION);
	
	_vector vLook = vTargetPosition - vPosition;
	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector vUp = XMVector3Cross(vLook, vRight);

	_float3 vScale = Get_Scale();

	Set_State(STATE::STATE_RIGHT,	XMVector3Normalize(vRight) * vScale.x);
	Set_State(STATE::STATE_UP,		XMVector3Normalize(vUp) * vScale.y);
	Set_State(STATE::STATE_LOOK,	XMVector3Normalize(vLook) * vScale.z);
}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	// 항등 상태에서 어떤 각도값으로 고정시키고 싶음.
	_float3 vScale = Get_Scale();

	_vector vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
	_vector vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;

	_matrix RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	Set_State(STATE::STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE::STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE::STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Rotation(_float3 vDegrees)
{
	// 항등 상태에서 어떤 각도값으로 고정시키고 싶음.
	_float3 vScale = Get_Scale();

	_vector vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
	_vector vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;
	
	_matrix RotationMatrixZ = XMMatrixRotationZ(XMConvertToRadians(vDegrees.z));
	_matrix RotationMatrixX = XMMatrixRotationX(XMConvertToRadians(vDegrees.x));
	_matrix RotationMatrixY = XMMatrixRotationY(XMConvertToRadians(vDegrees.y));
	
	_matrix RotationMatrix = RotationMatrixZ * RotationMatrixX * RotationMatrixY;

	Set_State(STATE::STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE::STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE::STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Turn(_fvector vAxis, _double dTimeDelta)
{
	_vector vRight = Get_State(STATE::STATE_RIGHT);
	_vector vUp = Get_State(STATE::STATE_UP);
	_vector vLook = Get_State(STATE::STATE_LOOK);

	_matrix RotationMatrix = XMMatrixRotationAxis(XMVector3Normalize(vAxis), static_cast<_float>(m_TransformDesc.dRotationPerSec * dTimeDelta));

	Set_State(STATE::STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE::STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE::STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

CTransform* CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTransform* pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CTransform");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CTransform::Clone(void* pArg)
{
	CTransform* pInstance = new CTransform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CTransform");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTransform::Free()
{
	__super::Free();
}
