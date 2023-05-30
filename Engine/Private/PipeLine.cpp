#include "..\Public\PipeLine.h"

IMPLEMENT_SINGLETON(CPipeLine)

void CPipeLine::Set_Transform(D3DTRANSFORMSTATE eTransformState, _fmatrix TransformStateMatrix)
{
	XMStoreFloat4x4(&m_TransformMatrix[eTransformState], TransformStateMatrix);
}

_matrix CPipeLine::Get_TransformMatrix(D3DTRANSFORMSTATE eTransformState)
{
	return XMLoadFloat4x4(&m_TransformMatrix[eTransformState]);
}

_float4x4* CPipeLine::Get_TransformFloat4x4(D3DTRANSFORMSTATE eTransformState)
{
	return &m_TransformMatrix[eTransformState];
}

_matrix CPipeLine::Get_TransformMatrix_Inverse(D3DTRANSFORMSTATE eTransformState)
{
	return XMLoadFloat4x4(&m_TransformMatrix_Inverse[eTransformState]);
}

_float4x4* CPipeLine::Get_TransformFloat4x4_Inverse(D3DTRANSFORMSTATE eTransformState)
{
	return &m_TransformMatrix_Inverse[eTransformState];
}

void CPipeLine::Tick()
{
	for (_uint i = 0; i < D3DTS_END; i++)
	{
		XMStoreFloat4x4(&m_TransformMatrix_Inverse[i], XMMatrixInverse(nullptr, Get_TransformMatrix(static_cast<D3DTRANSFORMSTATE>(i))));
	}
}

void CPipeLine::Free()
{
}
