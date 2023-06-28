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
	_bool IsMouseInClient(ID3D11DeviceContext* pContext, HWND hWnd);
	// ���͸� ������ �������� ����, y�������� ġȯ.
	HRESULT VectorToLineXZ(_vector vPosition, _vector vDirection, _float* pA, _float* pB);

public:
	virtual void Free() override;
};

END