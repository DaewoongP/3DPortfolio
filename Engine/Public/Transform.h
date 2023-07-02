#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };
	
public:
	typedef struct tagTransformDesc
	{
		tagTransformDesc() = default;
		tagTransformDesc(_double _SpeedPerSec, _double _RoationPerSec)
			: dSpeedPerSec{ _SpeedPerSec }
			, dRotationPerSec{ _RoationPerSec } { }
		_double		dSpeedPerSec = { 0.0 };
		_double		dRotationPerSec = { 0.0 };
	}TRANSFORMDESC;

private:
	explicit CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	_float3 Get_Velocity() const { return m_vVelocity; }
	TRANSFORMDESC Get_Desc() const { return m_TransformDesc; }
	_vector Get_State(STATE _eState) 
	{
		return XMLoadFloat4x4(&m_WorldMatrix).r[static_cast<_uint>(_eState)];
	}
	_float3 Get_Scale();
	const _float4x4* Get_WorldFloat4x4() const { return &m_WorldMatrix; }
	_matrix Get_WorldMatrix() const { return XMLoadFloat4x4(&m_WorldMatrix); }
	const _matrix Get_WorldMatrix_Inverse() const
	{
		return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix));
	}
	void Set_State(STATE _eState, _fvector _vState);
	void Set_Scale(const _float3& vScale);
	void Set_Desc(TRANSFORMDESC TransformDesc) { m_TransformDesc = TransformDesc; }
	void Set_WorldMatrix(_fmatrix WorldMatrix) { XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix); }
	// 최대 속도제한 설정
	void Set_LimitVelocity(_float fLimit) { m_fLimitVelocity = fLimit; }
	void Set_Speed(_double dSpeed) { m_TransformDesc.dSpeedPerSec = dSpeed; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual HRESULT Reset() override;

public:
	void RigidBody(_double dTimeDelta);
	void Check_Move(_fvector vCurrentPosition, _fvector vVelocity);
	void Check_Cell();
	void Move_Direction(_fvector vMoveDir, _double dTimeDelta);
	void Go_Straight(_double dTimeDelta);
	void Go_Backward(_double dTimeDelta);
	void Go_Left(_double dTimeDelta);
	void Go_Right(_double dTimeDelta);
	void Chase(_fvector vTargetPosition, _double dTimeDelta, _float fMinDistance = 0.1f);
	void LookAt(_fvector vTargetPosition);
	// 각도값 고정
	// 축을 기반으로 한 로테이션
	void Rotation(_fvector vAxis, _float fRadian);
	// 각도값 고정
	// 각각의 X,Y,Z 의 각도 값을 받아 처리.
	void Rotation(_float3 vDegrees);
	void Turn(_fvector vAxis, _double dTimeDelta);
	// 시간당 각도값으로 회전하는 함수
	void Turn(_fvector vAxis, _float fRadian, _double dTimeDelta);
	// 점프
	void Jump(_float fJumpForce, _double dTimeDelta);
	void Jump(_fvector vDir, _float fJumpForce, _double dTimeDelta);
	// 리지드 바디 사용
	void Use_RigidBody(class CNavigation* pNavigation, _float fLimitVelocity = 1.f, _float fMass = 5.f, _float fResistance = 30.f);
	_bool IsJumping() const { return m_isJumping; }
	void Crouch(_bool isCrouching, _double dTimeDelta, _float fCrouchSpeed, _float fCrouchSize = 2.f);
	void WallRun(_float fWallRunY, _fvector vWallRunDirection);
	void ZeroVelocity();

private:
	TRANSFORMDESC	m_TransformDesc;
	class CNavigation* m_pNavigation = { nullptr };

private: // 공용적인 변수값
	_float4x4		m_WorldMatrix;
	_bool			m_isRigidBody = { false };
	_float3			m_vGravity = _float3(0.f, -2.f, 0.f);
	// 속도
	_float3			m_vVelocity;
	// 힘
	_float3			m_vForce;
	// 가속도
	_float3			m_vAccel;
	// 바닥 Y값.
	_float			m_fOriginGroundY = 0.f;
	// 현재 셀 상태값
	CELLFLAG		m_eCurrentCellFlag;

private: // 객체 변수값.
	// 최대속도 제한
	_float			m_fLimitVelocity = 0.f;
	// 질량
	_float			m_fMass = 0.f;
	// 저항
	_float			m_fResistance = 0.f;
	// Crouch 처리용 (Player)
	_float			m_fGroundY = 0.f;
	_bool			m_isJumping = { false };

public:
	static CTransform* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END