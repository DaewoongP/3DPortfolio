#pragma once
#include "Base.h"

BEGIN(Engine)

class CPipeLine final : public CBase
{
	DECLARE_SINGLETON(CPipeLine)

public:
	enum D3DTRANSFORMSTATE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_END };

public:
	explicit CPipeLine() = default;
	virtual ~CPipeLine() = default;

public:
	void Set_Transform(D3DTRANSFORMSTATE eTransformState, _fmatrix TransformStateMatrix);

	_matrix Get_TransformMatrix(D3DTRANSFORMSTATE eTransformState);
	_float4x4* Get_TransformFloat4x4(D3DTRANSFORMSTATE eTransformState);
	_matrix Get_TransformMatrix_Inverse(D3DTRANSFORMSTATE eTransformState);
	_float4x4* Get_TransformFloat4x4_Inverse(D3DTRANSFORMSTATE eTransformState);

public:
	void Tick();

private:
	_float4x4				m_TransformMatrix[D3DTS_END];
	_float4x4				m_TransformMatrix_Inverse[D3DTS_END];
	_float4					m_vCameraPos;

public:
	virtual void Free() override;
};

END