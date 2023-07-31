#pragma once
#include "VIBuffer.h"
// 동적 버텍스 버퍼를 사용하여 버퍼를 쭉 앞의 배열로 움직여 줘야한다.
BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect_Trail final : public CVIBuffer
{
private:
	explicit CVIBuffer_Rect_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CVIBuffer_Rect_Trail(const CVIBuffer_Rect_Trail& rhs);
	virtual ~CVIBuffer_Rect_Trail() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iTrailNum, _float fDistance);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float3 vLocalHighPosition, _float3 vLocalLowPosition, _fmatrix WorldMatrix, _double dTimeDelta);

private:
	_uint		m_iTrailNum = { 0 };
	_float		m_fDistance = { 0.f };

public:
	static CVIBuffer_Rect_Trail* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _uint iTrailNum, _float fDistance);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END