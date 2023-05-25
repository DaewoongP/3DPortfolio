#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum class STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_END };
public:
	typedef struct tagTransformDesc
	{
		tagTransformDesc(_double _SpeedPerSec, _double _RoationPerSec)
			: dSpeedPerSec{ _SpeedPerSec }
			, dRotationPerSec{ _RoationPerSec }
		{

		}
		_double		dSpeedPerSec = { 0.0 };
		_double		dRotationPerSec = { 0.0 };
	}TRANSFORMDESC;
private:
	explicit CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	_vector Get_State(STATE _eState) {
		return XMLoadFloat4x4(&m_WorldMatrix).r[static_cast<_uint>(_eState)];
	}

	void Set_State(STATE _eState, _fvector _vState)
	{
		_float4		vState;
		XMStoreFloat4(&vState, _vState);
		memcpy(&m_WorldMatrix.m[static_cast<_uint>(_eState)][0], &vState, sizeof vState);
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

private:
	_float4x4		m_WorldMatrix;
	TRANSFORMDESC	m_TransformDesc;

public:
	static CTransform* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END