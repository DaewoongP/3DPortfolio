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
	, m_fResistance(rhs.m_fResistance)
	, m_fStepOffset(rhs.m_fStepOffset)
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

void CTransform::Use_RigidBody(CNavigation* pNavigation, _float fLimitVelocity, _float fMass, _float fResistance)
{
	m_pNavigation = pNavigation;
	Safe_AddRef(m_pNavigation);

	m_isRigidBody = true;

	m_fLimitVelocity = fLimitVelocity;
	m_fMass = fMass;
	m_fResistance = fResistance;
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
	ZEROMEM(&m_eCurrentCellFlag);
	m_fGroundY = 4.f;
	m_fOriginGroundY = 4.f;
	m_fStepOffset = 1.1f;

	return S_OK;
}

void CTransform::Tick(_double dTimeDelta)
{
	RigidBody(dTimeDelta);

	__super::Tick(dTimeDelta);
}

HRESULT CTransform::Reset()
{
	// 초기값 세팅으로 변경.

	ZEROMEM(&m_vVelocity);
	ZEROMEM(&m_vForce);
	ZEROMEM(&m_vAccel);
	ZEROMEM(&m_eCurrentCellFlag);
	
	m_fOriginGroundY = 4.f;
	m_fGroundY = m_fOriginGroundY;
	m_fLimitVelocity = 1.f;

	return S_OK;
}

void CTransform::RigidBody(_double dTimeDelta)
{
	if (false == m_isRigidBody ||
		nullptr == m_pNavigation)
		return;

	// 중력 F = G * M;
	XMStoreFloat3(&m_vForce,
		XMLoadFloat3(&m_vForce) + XMLoadFloat3(&m_vGravity) * m_fMass);

	// 저항 F = -K * V
	XMStoreFloat3(&m_vForce,
		XMLoadFloat3(&m_vForce) + XMLoadFloat3(&m_vVelocity) * m_fResistance * -1.f);

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
}

void CTransform::Check_Move(_fvector vCurrentPosition, _fvector vVelocity)
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
	isMove = m_pNavigation->Is_Move(vXZPosition, &vNormal, &m_eCurrentCellFlag);

	// 원래 포지션의 Y값에서 셀의 Y값 만큼 더해줌.
	if (true == isMove &&
		(CELL_MOVE == m_eCurrentCellFlag || CELL_SLIDE == m_eCurrentCellFlag))
	{		
		if (false == Check_CellY(vXZPosition, vCurrentPosition))
		{
			m_eCurrentCellFlag = m_ePreviousCellFlag;

			Set_State(STATE_POSITION, XMVectorSetY(vCurrentPosition, XMVectorGetY(vCurrentPosition) + XMVectorGetY(vVelocity)));
			return;
		}
	}

	_uint iExcept = { 0 };

	while (false == isMove)
	{
		vXZPosition = vXZOriginPosition +
			(vDir - XMLoadFloat3(&vNormal) *
				(XMVector3Dot(vDir, XMLoadFloat3(&vNormal)))) *
			XMVectorGetX(XMVector3Length(vXZVelocity));

		isMove = m_pNavigation->Is_Move(vXZPosition, &vNormal, &m_eCurrentCellFlag);

		if (++iExcept > 100)
		{
			vXZPosition = vXZOriginPosition;
			break;
		}
	}

	if (true == isMove &&
		(CELL_MOVE == m_eCurrentCellFlag || CELL_SLIDE == m_eCurrentCellFlag))
	{
		Check_CellY(vXZPosition, vCurrentPosition);
	}
	

	_vector vMovedPosition = XMVectorSet(
		XMVectorGetX(vXZPosition), 
		XMVectorGetY(vCurrentPosition) + XMVectorGetY(vVelocity),
		XMVectorGetZ(vXZPosition), 
		1.f);

	Check_Cell();

	Set_State(STATE_POSITION, vMovedPosition);
}

void CTransform::Check_Cell()
{
	if (m_ePreviousCellFlag != m_eCurrentCellFlag)
	{
		switch (m_eCurrentCellFlag)
		{
		case Engine::CELL_MOVE:
			m_fGroundY = m_fOriginGroundY;
			break;
		case Engine::CELL_FALL:
			m_fGroundY = -999.f;
			break;
		}

		m_ePreviousCellFlag = m_eCurrentCellFlag;
	}
}

_bool CTransform::Check_CellY(_fvector vXZPosition, _fvector vCurrentPosition)
{
	_float fCellY = m_fOriginGroundY + m_pNavigation->Get_CurrentCellY(vCurrentPosition);

	if (true == m_isCrouch &&
		CELL_SLIDE != m_eCurrentCellFlag)
		fCellY -= m_fOriginGroundY - m_fGroundY;

	if (fCellY > XMVectorGetY(vCurrentPosition) + m_fStepOffset)
	{
		return false;
	}
	else
	{
		m_fGroundY = fCellY;
		return true;
	}
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

_bool CTransform::Chase(_fvector vTargetPosition, _double dTimeDelta, _float fMinDistance)
{
	_vector vPosition = Get_State(STATE::STATE_POSITION);
	_vector vDir = vTargetPosition - vPosition;

	if (fMinDistance <= XMVectorGetX(XMVector3Length(vDir)))
		Move_Direction(vDir, dTimeDelta);
	else
		return false;

	return true;
}

void CTransform::LookAt(_fvector vTargetPosition, _bool bDeleteY)
{
	_vector vPosition = Get_State(STATE::STATE_POSITION);
	
	_vector vLook = vTargetPosition - vPosition;
	if (true == bDeleteY)
		vLook.m128_f32[1] = 0.f;

	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector vUp = XMVector3Cross(vLook, vRight);

	_float3 vScale = Get_Scale();

	Set_State(STATE::STATE_RIGHT,	XMVector3Normalize(vRight) * vScale.x);
	Set_State(STATE::STATE_UP,		XMVector3Normalize(vUp) * vScale.y);
	Set_State(STATE::STATE_LOOK,	XMVector3Normalize(vLook) * vScale.z);
}

void CTransform::Rotation(_fvector vAxis, _float fRadian, _bool bUseCurrentRotation)
{
	// 항등 상태에서 어떤 각도값으로 고정시키고 싶음.
	_float3 vScale = Get_Scale();

	_vector vRight, vUp, vLook;
	if (true == bUseCurrentRotation)
	{
		vRight = Get_State(STATE::STATE_RIGHT);
		vUp = Get_State(STATE::STATE_UP);
		vLook = Get_State(STATE::STATE_LOOK);
	}
	else
	{
		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;
	}

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

	_matrix RotationMatrixX = XMMatrixRotationX(XMConvertToRadians(vDegrees.x));
	_matrix RotationMatrixY = XMMatrixRotationY(XMConvertToRadians(vDegrees.y));
	_matrix RotationMatrixZ = XMMatrixRotationZ(XMConvertToRadians(vDegrees.z));
	
	_matrix RotationMatrix = RotationMatrixX * RotationMatrixY * RotationMatrixZ;

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
#ifdef _DEBUG
	//m_isJumping = false;
#endif // _DEBUG


	m_vForce.y += fJumpForce / (_float)dTimeDelta;
}

void CTransform::Jump(_fvector vDir, _float fJumpForce, _double dTimeDelta)
{
	m_isJumping = true;

	XMStoreFloat3(&m_vForce, XMLoadFloat3(&m_vForce) + vDir * fJumpForce / (_float)dTimeDelta);
}


void CTransform::Crouch(_bool isCrouching, _double dTimeDelta, _float fCrouchSpeed, _float fCrouchSize, _float fSlidingSpeed)
{
	// 점프상태일경우 리턴
	if (true == m_isJumping ||
		CELL_FALL == m_eCurrentCellFlag)
		return;

	m_isCrouch = isCrouching;

	if (true == isCrouching &&
		CELL_SLIDE == m_eCurrentCellFlag)
	{
		XMStoreFloat3(&m_vVelocity, XMLoadFloat3(&m_vVelocity) * fSlidingSpeed);
	}

	m_fCrouchSize = fCrouchSpeed * fCrouchSize * (_float)dTimeDelta;

	// 앉기를 눌러서 천천히 내려앉음.
	if (true == isCrouching)
	{
		m_fGroundY -= m_fCrouchSize;
	}
	else
	{
		m_fGroundY += m_fCrouchSize;
	}

	// 예외처리
	if (m_fGroundY < fCrouchSize)
		m_fGroundY = fCrouchSize;
	if (m_fGroundY > m_fOriginGroundY)
		m_fGroundY = m_fOriginGroundY;
}

void CTransform::WallRun(_float fWallRunY, _fvector vWallRunDirection)
{
	m_isJumping = false;

	_vector vPos = XMVectorSet(m_WorldMatrix._41, fWallRunY, m_WorldMatrix._43, 1.f);
	
	XMStoreFloat3(&m_vVelocity, vWallRunDirection);

	Set_State(CTransform::STATE_POSITION, vPos);
}

void CTransform::ZeroVelocity()
{
	XMStoreFloat3(&m_vVelocity, XMVectorZero());
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
