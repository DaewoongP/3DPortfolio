#include "..\Public\Transform.h"
#include "Navigation.h"
#include "Calculator.h"

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
	, m_isRigidBody(rhs.m_isRigidBody)
	, m_vGravity(rhs.m_vGravity)
	, m_vVelocity(rhs.m_vVelocity)
	, m_fLimitVelocity(rhs.m_fLimitVelocity)
	, m_vForce(rhs.m_vForce)
	, m_vAccel(rhs.m_vAccel)
	, m_fMass(rhs.m_fMass)
	, m_fGroundY(rhs.m_fGroundY)
	, m_fOriginGroundY(rhs.m_fOriginGroundY)
	, m_isJumping(rhs.m_isJumping)
	, m_fAirResistance(rhs.m_fAirResistance)
	, m_fGroundResistance(rhs.m_fGroundResistance)
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

void CTransform::Use_RigidBody(CNavigation* pNavigation)
{
	m_pNavigation = pNavigation;
	Safe_AddRef(m_pNavigation);

	m_isRigidBody = true;
}

HRESULT CTransform::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_TransformDesc, pArg, sizeof m_TransformDesc);

	ZEROMEM(&m_vVelocity);
	ZEROMEM(&m_vForce);
	ZEROMEM(&m_vAccel);

	m_fGroundY = 4.f;
	m_fOriginGroundY = 4.f;
	m_fLimitVelocity = 1.f;

	return S_OK;
}

void CTransform::Tick(_double dTimeDelta)
{
	if (false == m_isRigidBody || 
		nullptr == m_pNavigation)
		return;

	// 중력 F = G * M;
	XMStoreFloat3(&m_vForce,
		XMLoadFloat3(&m_vForce) + XMLoadFloat3(&m_vGravity) * m_fMass);

	// 저항 F = -K * V
	_float fResistance = 0.f;

	if (true == m_isJumping)
		fResistance = m_fAirResistance;
	else
		fResistance = m_fGroundResistance;

	XMStoreFloat3(&m_vForce, 
		XMLoadFloat3(&m_vForce) + XMLoadFloat3(&m_vVelocity) * fResistance * -1.f);

	// 가속도  A = F / M
	XMStoreFloat3(&m_vAccel,
		XMLoadFloat3(&m_vForce) / m_fMass);

	// 속도  V = A * dt
	XMStoreFloat3(&m_vVelocity,
		XMLoadFloat3(&m_vVelocity) + XMLoadFloat3(&m_vAccel) * (_float)dTimeDelta);


	Check_Move(Get_State(STATE::STATE_POSITION), XMLoadFloat3(&m_vVelocity));

	if (m_WorldMatrix._42 <= m_fGroundY)
	{
		m_vAccel.y = 0.f;
		m_vVelocity.y = 0.f;
		m_WorldMatrix._42 = m_fGroundY;
		m_isJumping = false;
	}

	// 처리한 힘 초기화.
	ZEROMEM(&m_vForce);

	__super::Tick(dTimeDelta);
}

void CTransform::Check_Move(_vector vCurrentPosition, _vector vVelocity)
{
	_vector vXZOriginPosition = XMVectorSet(XMVectorGetX(vCurrentPosition), 0.f, XMVectorGetZ(vCurrentPosition), 1.f);
	_vector vXZVelocity = XMVectorSet(XMVectorGetX(vVelocity), 0.f, XMVectorGetZ(vVelocity), 0.f);
	_vector vDir = XMVector3Normalize(vXZVelocity);

	_vector vXZPosition;
	vXZPosition = vXZOriginPosition + vXZVelocity;

	_bool		isMove = true;
	_float3		vNormal;
	_float3		vLineStartPoint, vLineDirection;
	ZEROMEM(&vNormal);
	ZEROMEM(&vLineStartPoint);
	ZEROMEM(&vLineDirection);

	// 움직일수 있는지 체크
	// X,Z로만 체크.
	isMove = m_pNavigation->Is_Move(vXZPosition, &vLineStartPoint, &vLineDirection, &vNormal);

	if (false == isMove)
	{
		// 움직일 수 있으면 직선의 방정식 교점을 통해서 그 위치로부터 슬라이딩벡터를 추가하여 처리.
		// 그럼 못움직일때의 선분 벡터도 구해와야함.
		
		CCalculator* pCalculator = CCalculator::GetInstance();
		Safe_AddRef(pCalculator);
		// 기울기, Z절편
		_float fA = 0.f;
		_float fB = 0.f;

		pCalculator->VectorToLineXZ(XMLoadFloat3(&vLineStartPoint), XMLoadFloat3(&vLineDirection), &fA, &fB);

		vXZPosition = vXZOriginPosition +
			(vDir - XMLoadFloat3(&vNormal) * 
			(XMVector3Dot(vDir, XMLoadFloat3(&vNormal)))) * 
			XMVectorGetX(XMVector3Length(vXZVelocity));

		Safe_Release(pCalculator);
	}

	_vector vMovedPosition = XMVectorSet(
		XMVectorGetX(vXZPosition), 
		XMVectorGetY(vCurrentPosition) + XMVectorGetY(vVelocity), 
		XMVectorGetZ(vXZPosition), 
		1.f);

	Set_State(STATE_POSITION, vMovedPosition);
}

void CTransform::Move_Direction(_fvector vMoveDir, _double dTimeDelta)
{
	_vector vDir;

	if (true == m_isRigidBody)
	{
		// 속도처리 있는 움직임.

		vDir = XMVector3Normalize(XMVectorSet(XMVectorGetX(vMoveDir), 0.f, XMVectorGetZ(vMoveDir), 0.f));

		_vector vVelocity = vDir * (_float)m_TransformDesc.dSpeedPerSec * (_float)dTimeDelta;

		XMStoreFloat3(&m_vVelocity, XMLoadFloat3(&m_vVelocity) + vVelocity);

		// x,z로만 상한선 처리.
		_vector vLimitXZ = XMVectorSet(m_vVelocity.x, 0.f, m_vVelocity.z, 1.f);

		if (XMVectorGetX(XMVector3Length(vLimitXZ)) > m_fLimitVelocity)
		{
			vLimitXZ = XMVector3Normalize(vLimitXZ) * m_fLimitVelocity;
			m_vVelocity.x = XMVectorGetX(vLimitXZ);
			m_vVelocity.z = XMVectorGetZ(vLimitXZ);
		}
	}
	else
	{
		// 일반적인 움직임.
		vDir = XMVector3Normalize(vMoveDir);

		_vector		vPosition = Get_State(STATE_POSITION);

		vPosition += vDir * (_float)m_TransformDesc.dSpeedPerSec * (_float)dTimeDelta;

		Set_State(STATE_POSITION, vPosition);
	}
}

void CTransform::Go_Straight(_double dTimeDelta)
{
	_vector vLook = Get_State(STATE::STATE_LOOK);
	Move_Direction(vLook, dTimeDelta);
}

void CTransform::Go_Backward(_double dTimeDelta)
{
	_vector vLook = Get_State(STATE::STATE_LOOK);
	Move_Direction(-vLook, dTimeDelta);
}

void CTransform::Go_Left(_double dTimeDelta)
{
	_vector vRight = Get_State(STATE::STATE_RIGHT);
	Move_Direction(-vRight, dTimeDelta);
}

void CTransform::Go_Right(_double dTimeDelta)
{
	_vector vRight = Get_State(STATE::STATE_RIGHT);
	Move_Direction(vRight, dTimeDelta);
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

void CTransform::Turn(_fvector vAxis, _float fRadian, _double dTimeDelta)
{
	_vector vRight = Get_State(STATE::STATE_RIGHT);
	_vector vUp = Get_State(STATE::STATE_UP);
	_vector vLook = Get_State(STATE::STATE_LOOK);

	_matrix RotationMatrix = XMMatrixRotationAxis(XMVector3Normalize(vAxis), static_cast<_float>(fRadian * dTimeDelta));

	Set_State(STATE::STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE::STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE::STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Jump(_float fJumpForce, _double dTimeDelta)
{
	m_isJumping = true;

	m_vForce.y += fJumpForce / (_float)dTimeDelta;
}

void CTransform::Crouch(_bool isCrouching, _double dTimeDelta, _float fCrouchSpeed, _float fCrouchSize)
{
	// 점프상태일경우 리턴
	if (true == m_isJumping)
		return;

	// 앉기를 눌러서 천천히 내려앉음.
	if (true == isCrouching)
	{
		m_fGroundY -= fCrouchSpeed * fCrouchSize * (_float)dTimeDelta;
	}
	else
	{
		m_fGroundY += fCrouchSpeed * fCrouchSize * (_float)dTimeDelta;
	}

	// 예외처리
	if (m_fGroundY < fCrouchSize)
		m_fGroundY = fCrouchSize;
	if (m_fGroundY > m_fOriginGroundY)
		m_fGroundY = m_fOriginGroundY;
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

	Safe_Release(m_pNavigation);
}
