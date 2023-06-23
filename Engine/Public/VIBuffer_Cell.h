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
	vector<class CBounding_Sphere*> Get_BoundingSpheres() const { return m_BoundingSpheres; }

public:
	virtual HRESULT Initialize_Prototype(_float3 * pPoints);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double dTimeDelta) override;
#ifdef _DEBUG
	virtual HRESULT Render_Sphere();
#endif // _DEBUG

public:
	HRESULT Begin(_float3* pPoints);
	HRESULT End();

private:
	class CBounding_Sphere*			m_pOriginSphere = { nullptr };
	vector<_float3>					m_VertexPositions;
	vector<class CBounding_Sphere*>	m_BoundingSpheres;

public:
	static CVIBuffer_Cell* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _float3 * pPoints);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END