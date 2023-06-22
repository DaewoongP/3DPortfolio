#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Cell final : public CVIBuffer
{
private:
	explicit CVIBuffer_Cell(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CVIBuffer_Cell(const CVIBuffer_Cell& rhs);
	virtual ~CVIBuffer_Cell() = default;

public:
	vector<_float3> Get_VertexPositions() const { return m_VertexPositions; }

public:
	virtual HRESULT Initialize_Prototype(const _float3 * pPoints);
	virtual HRESULT Initialize(void* pArg) override;

private:
	vector<_float3>			m_VertexPositions;

public:
	static CVIBuffer_Cell* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _float3 * pPoints);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END