#include "..\Public\PipeLine.h"

IMPLEMENT_SINGLETON(CPipeLine)

CPipeLine::CPipeLine()
{
	for (_uint i = 0; i < D3DTS_END; ++i)
	{
		XMStoreFloat4x4(&m_TransformMatrix[i], XMMatrixIdentity());
		XMStoreFloat4x4(&m_LightDepthMatrix[i], XMMatrixIdentity());
		XMStoreFloat4x4(&m_TransformMatrix_Inverse[i], XMMatrixIdentity());
	}

	ZEROMEM(&m_vCameraPos);
}

void CPipeLine::Set_Transform(D3DTRANSFORMSTATE eTransformState, _fmatrix TransformStateMatrix)
{
	XMStoreFloat4x4(&m_TransformMatrix[eTransformState], TransformStateMatrix);
}

void CPipeLine::Set_LightDepthMatrix(D3DTRANSFORMSTATE eTransformState, _fmatrix LightStateMatrix)
{
	XMStoreFloat4x4(&m_LightDepthMatrix[eTransformState], LightStateMatrix);
}

_matrix CPipeLine::Get_TransformMatrix(D3DTRANSFORMSTATE eTransformState)
{	
	return XMLoadFloat4x4(&m_TransformMatrix[eTransformState]);
}

_float4x4* CPipeLine::Get_TransformFloat4x4(D3DTRANSFORMSTATE eTransformState)
{
	return &m_TransformMatrix[eTransformState];
}

_float4x4* CPipeLine::Get_LightDepthFloat4x4(D3DTRANSFORMSTATE eTransformState)
{
	return &m_LightDepthMatrix[eTransformState];
}

_matrix CPipeLine::Get_TransformMatrix_Inverse(D3DTRANSFORMSTATE eTransformState)
{
	return XMLoadFloat4x4(&m_TransformMatrix_Inverse[eTransformState]);
}

_float4x4* CPipeLine::Get_TransformFloat4x4_Inverse(D3DTRANSFORMSTATE eTransformState)
{
	return &m_TransformMatrix_Inverse[eTransformState];
}

_float4* CPipeLine::Get_CamPosition()
{
	return &m_vCameraPos;
}

_float* CPipeLine::Get_CamFar()
{
	return &m_fCameraFar;
}

_float* CPipeLine::Get_LightFar()
{
	return &m_fLightFar;
}

void CPipeLine::Tick()
{
	for (_uint i = 0; i < D3DTS_END; i++)
	{
		XMStoreFloat4x4(&m_TransformMatrix_Inverse[i], XMMatrixInverse(nullptr, Get_TransformMatrix(static_cast<D3DTRANSFORMSTATE>(i))));
	}
	
	memcpy(&m_vCameraPos, &m_TransformMatrix_Inverse[D3DTS_VIEW].m[3][0], sizeof m_vCameraPos);
}

void CPipeLine::Free()
{
}
