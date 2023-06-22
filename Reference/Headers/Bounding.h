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
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pBoundingDesc) PURE;
	virtual void Tick(_fmatrix WorldMatrix) PURE;

#ifdef _DEBUG
public:
	// effect, batch 등을 통해 렌더링 준비
	HRESULT Begin();
	virtual HRESULT Render(_fvector vColor = DirectX::Colors::Green) PURE;
	HRESULT End();
#endif // _DEBUG

protected:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

protected:
	PrimitiveBatch<DirectX::VertexPositionColor>* m_pPrimitiveBatch = { nullptr };
	DirectX::BasicEffect*	m_pEffect = { nullptr };
	ID3D11InputLayout*		m_pInputLayout = { nullptr };
	
protected:
	_bool					m_isCloned = { false };

public:
	virtual CBounding* Clone(void* pBoundingDesc) PURE;
	virtual void Free() override;
};

END