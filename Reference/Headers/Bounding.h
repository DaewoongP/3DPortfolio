#pragma once
#include "Base.h"

#ifdef _DEBUG
#include "DebugDraw.h"
#endif // _DEBUG

BEGIN(Engine)

class CBounding abstract : public CBase
{
public:
	typedef struct tagBoundingDesc
	{
		_float3		vPosition;
	}BOUNDINGDESC;

protected:
	explicit CBounding(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBounding(const CBounding& rhs);
	virtual ~CBounding() = default;

public:
	virtual void Tick(_fmatrix TransformMatrix) PURE;

#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<DirectX::VertexPositionColor>* pBatch) PURE;
#endif // _DEBUG

protected:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

public:
	virtual CBounding* Clone(void* pBoundingDesc) PURE;
	virtual void Free() override;
};

END