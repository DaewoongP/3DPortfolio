#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instance abstract : public CVIBuffer
{
protected:
	explicit CVIBuffer_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CVIBuffer_Instance(const CVIBuffer_Instance& rhs);
	virtual ~CVIBuffer_Instance() = default;

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