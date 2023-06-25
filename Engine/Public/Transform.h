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

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Move_Direction(_fvector vMoveDir, _double dTimeDelta, class CNavigation* pNavigation = nullptr);
	void Go_Straight(_double dTimeDelta, class CNavigation* pNavigation = nullptr);
	void Go_Backward(_double dTimeDelta, class CNavigation* pNavigation = nullptr);
	void Go_Left(_double dTimeDelta, class CNavigation* pNavigation = nullptr);
	void Go_Right(_double dTimeDelta, class CNavigation* pNavigation = nullptr);
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
	
private:
	TRANSFORMDESC	m_TransformDesc;

private:
	_float4x4		m_WorldMatrix;

public:
	static CTransform* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END