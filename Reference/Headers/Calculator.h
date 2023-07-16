#pragma once
#include "Base.h"

BEGIN(Engine)

class CCalculator final : public CBase
{
	DECLARE_SINGLETON(CCalculator)

public:
	explicit CCalculator();
	virtual ~CCalculator() = default;

public:
	HRESULT Get_MouseRay(ID3D11DeviceContext* pContext, HWND hWnd, _fmatrix PickingWorldMatrix_Inverse, _Inout_ _float4* vRayPos, _Inout_ _float4* vRayDir);
	HRESULT Get_WorldMouseRay(ID3D11DeviceContext* pContext, HWND hWnd, _Inout_ _float4* vRayPos, _Inout_ _float4* vRayDir);
	_bool IsMouseInClient(ID3D11DeviceContext* pContext, HWND hWnd);
	// 벡터를 직선의 방정식의 기울기, y절편으로 치환.
	HRESULT VectorToLineXZ(_vector vPosition, _vector vDirection, _float* pA, _float* pB);
	// 0~1 사이의 3차원 큐브 상의 위치 좌표 대입.
	_float Perlin_Noise(_fvector vPixel);

public:
	virtual void Free() override;
};

END