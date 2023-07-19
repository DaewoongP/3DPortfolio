#include "..\Public\Calculator.h"
#include "PipeLine.h"
#include <limits>

IMPLEMENT_SINGLETON(CCalculator)

CCalculator::CCalculator()
{
}

HRESULT CCalculator::Get_MouseRay(ID3D11DeviceContext* pContext, HWND hWnd, _fmatrix PickingWorldMatrix_Inverse, _Inout_ _float4* vRayPos, _Inout_ _float4* vRayDir)
{
    D3D11_VIEWPORT ViewPort;
    UINT iNumViewPorts = 1;

    ZEROMEM(&ViewPort);
    if (nullptr == pContext)
        return E_FAIL;
    pContext->RSGetViewports(&iNumViewPorts, &ViewPort);

	POINT	pt{};
	GetCursorPos(&pt);
	ScreenToClient(hWnd, &pt);

	_vector		vMouse;
	vMouse = XMVectorSet(
		pt.x / (ViewPort.Width * 0.5f) - 1.f,
		pt.y / -(ViewPort.Height * 0.5f) + 1.f,
		0.f, 
		1.f);

	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);
	
	_matrix		ProjMatrix_Inverse;
	ProjMatrix_Inverse = pPipeLine->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_PROJ);
	vMouse = XMVector3TransformCoord(vMouse, ProjMatrix_Inverse);

	_matrix		ViewMatrix_Inverse;
	ViewMatrix_Inverse = pPipeLine->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW);

	Safe_Release(pPipeLine);

	_vector vPickPos, vPickDir;
	vPickPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	vPickDir = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	vPickDir = vMouse - vPickPos;
	vPickDir = XMVector3Normalize(vPickDir);

	vPickPos = XMVector3TransformCoord(vPickPos, ViewMatrix_Inverse);
	vPickDir = XMVector3TransformNormal(vPickDir, ViewMatrix_Inverse);

	vPickPos = XMVector3TransformCoord(vPickPos, PickingWorldMatrix_Inverse);
	vPickDir = XMVector3TransformNormal(vPickDir, PickingWorldMatrix_Inverse);

	XMStoreFloat4(vRayPos, vPickPos);
	XMStoreFloat4(vRayDir, vPickDir);

	return S_OK;
}

HRESULT CCalculator::Get_WorldMouseRay(ID3D11DeviceContext* pContext, HWND hWnd, _Inout_ _float4* vRayPos, _Inout_ _float4* vRayDir)
{
	D3D11_VIEWPORT ViewPort;
	UINT iNumViewPorts = 1;

	ZEROMEM(&ViewPort);
	if (nullptr == pContext)
		return E_FAIL;
	pContext->RSGetViewports(&iNumViewPorts, &ViewPort);

	POINT	pt{};
	GetCursorPos(&pt);
	ScreenToClient(hWnd, &pt);

	_vector		vMouse;
	vMouse = XMVectorSet(
		pt.x / (ViewPort.Width * 0.5f) - 1.f,
		pt.y / -(ViewPort.Height * 0.5f) + 1.f,
		0.f,
		1.f);

	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	_matrix		ProjMatrix_Inverse;
	ProjMatrix_Inverse = pPipeLine->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_PROJ);
	vMouse = XMVector3TransformCoord(vMouse, ProjMatrix_Inverse);

	_matrix		ViewMatrix_Inverse;
	ViewMatrix_Inverse = pPipeLine->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW);

	Safe_Release(pPipeLine);

	_vector vPickPos, vPickDir;
	vPickPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	vPickDir = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	vPickDir = vMouse - vPickPos;
	vPickDir = XMVector3Normalize(vPickDir);

	vPickPos = XMVector3TransformCoord(vPickPos, ViewMatrix_Inverse);
	vPickDir = XMVector3TransformNormal(vPickDir, ViewMatrix_Inverse);

	XMStoreFloat4(vRayPos, vPickPos);
	XMStoreFloat4(vRayDir, vPickDir);

	return S_OK;
}

_bool CCalculator::IsMouseInClient(ID3D11DeviceContext* pContext, HWND hWnd)
{
	D3D11_VIEWPORT ViewPort;
	UINT iNumViewPorts = 1;

	ZEROMEM(&ViewPort);
	if (nullptr == pContext)
		return false;
	pContext->RSGetViewports(&iNumViewPorts, &ViewPort);

	POINT	pt{};
	GetCursorPos(&pt);

	RECT rc{};
	GetWindowRect(hWnd, &rc);

	if (rc.left <= pt.x &&
		rc.right >= pt.x &&
		rc.top <= pt.y &&
		rc.bottom >= pt.y)
		return true;

	return false;
}

HRESULT CCalculator::VectorToLineXZ(_vector vPosition, _vector vDirection, _float* pA, _float* pB)
{
	// 기울기 
	_vector vSourPos = vPosition + vDirection;

	// Z증가량 / X증가량
	*pA = (_float)(XMVectorGetZ(vDirection)) / XMVectorGetX(vDirection);
	// Z 절편 
	*pB = XMVectorGetZ(vPosition) - (*pA) * XMVectorGetX(vPosition);

	return S_OK;
}

_float CCalculator::Perlin_Noise(_fvector vPixel)
{
	// 바운딩 그리드 생성.
	_vector vGridPoints[8];
	vGridPoints[0] = XMVectorSet(0.f, 1.f, 0.f, 1.f);
	vGridPoints[1] = XMVectorSet(1.f, 1.f, 0.f, 1.f);
	vGridPoints[2] = XMVectorSet(1.f, 0.f, 0.f, 1.f);
	vGridPoints[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	vGridPoints[4] = XMVectorSet(0.f, 1.f, 1.f, 1.f);
	vGridPoints[5] = XMVectorSet(1.f, 1.f, 1.f, 1.f);
	vGridPoints[6] = XMVectorSet(1.f, 0.f, 1.f, 1.f);
	vGridPoints[7] = XMVectorSet(0.f, 0.f, 1.f, 1.f);

	_vector vGradient[8];
	_vector vDistance[8];
	_float fInfluence[8];
	for (_uint i = 0; i < 8; ++i)
	{
		//vGradient[i] = vGridPoints[i] + XMVector3Normalize(XMVectorSet((rand() % 100) / (rand() % 100 + 1), (rand() % 100) / (rand() % 100 + 1), (rand() % 100) / (rand() % 100 + 1), 0.f));
		vDistance[i] = vPixel - vGridPoints[i];

		fInfluence[i] = XMVectorGetX(XMVector3Dot(vGradient[i], vDistance[i]));
	}

	return _float();
}

void CCalculator::Free()
{
}
