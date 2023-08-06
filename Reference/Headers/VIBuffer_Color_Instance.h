#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Color_Instance abstract : public CVIBuffer
{
public:
	typedef struct tagColorInstance
	{
		_float4x4	InstanceLocalMatrix;
		_float4		vInstanceColor;
	}COLORINSTANCE;

protected:
	explicit CVIBuffer_Color_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CVIBuffer_Color_Instance(const CVIBuffer_Color_Instance& rhs);
	virtual ~CVIBuffer_Color_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT Render();

protected:
	ID3D11Buffer*	m_pVBInstance = { nullptr };
	_uint			m_iInstanceStride = { 0 };
	_uint			m_iIndexCountPerInstance = { 0 };
	_uint			m_iNumInstance = { 0 };

public:
	virtual CComponent* Clone(void* pArg) PURE;
	virtual void Free() override;
};

END