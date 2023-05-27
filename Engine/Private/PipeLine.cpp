#include "..\Public\PipeLine.h"

IMPLEMENT_SINGLETON(CPipeLine)

CPipeLine::CPipeLine()
{
	ZeroMemory(&m_TransformState, sizeof m_TransformState * static_cast<_uint>(D3DTRANSFORMSTATE::D3DTS_END));
	ZeroMemory(&m_TransformState_Inverse, sizeof m_TransformState_Inverse * static_cast<_uint>(D3DTRANSFORMSTATE::D3DTS_END));
	ZeroMemory(&m_vCameraPos, sizeof m_vCameraPos);
}

void CPipeLine::Set_Transform(D3DTRANSFORMSTATE eTransformState, _fmatrix TransformStateMatrix)
{
	XMStoreFloat4x4(&m_TransformState[static_cast<_uint>(eTransformState)], TransformStateMatrix);
}

_matrix CPipeLine::Get_TransformMatrix(D3DTRANSFORMSTATE eTransformState)
{
	return XMLoadFloat4x4(&m_TransformState[static_cast<_uint>(eTransformState)]);
}

_float4x4 CPipeLine::Get_TransformFloat4x4(D3DTRANSFORMSTATE eTransformState)
{
	return m_TransformState[static_cast<_uint>(eTransformState)];
}

_matrix CPipeLine::Get_TransformMatrix_Inverse(D3DTRANSFORMSTATE eTransformState)
{
	return XMLoadFloat4x4(&m_TransformState_Inverse[static_cast<_uint>(eTransformState)]);

}

_float4x4 CPipeLine::Get_TransformFloat4x4_Inverse(D3DTRANSFORMSTATE eTransformState)
{
	return m_TransformState_Inverse[static_cast<_uint>(eTransformState)];
}

void CPipeLine::Tick()
{
	for (_uint i = 0; i < static_cast<_uint>(D3DTRANSFORMSTATE::D3DTS_END); ++i)
	{
		XMStoreFloat4x4(&m_TransformState_Inverse[i], 
			XMMatrixInverse(nullptr, Get_TransformMatrix(static_cast<D3DTRANSFORMSTATE>(i))));
	}

}

void CPipeLine::Free()
{
}
