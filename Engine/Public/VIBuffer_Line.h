#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Line final : public CVIBuffer
{
public:
	struct Line_Desc
	{
		_uint iNum = { 0 };
		const _float3* pLines = { nullptr };
	};
protected:
	explicit CVIBuffer_Line(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CVIBuffer_Line(const CVIBuffer_Line& rhs);
	virtual ~CVIBuffer_Line() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	static CVIBuffer_Line* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _uint iLineCnt, const _float3* pLocalPoints);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END

/*
그리고 싶은 라인의 개수,
라인의 로컬 위치값,
*/