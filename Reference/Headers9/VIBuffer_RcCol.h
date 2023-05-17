#pragma once
#include "VIBuffer.h"

BEGIN(Engine9)

class ENGINE_DLL CVIBuffer_RcCol final : public CVIBuffer
{
private:
	explicit CVIBuffer_RcCol(LPDIRECT3DDEVICE9 pDevice);
	explicit CVIBuffer_RcCol(const CVIBuffer_RcCol& rhs);
	virtual ~CVIBuffer_RcCol() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual HRESULT Render();

public:
	static CVIBuffer_RcCol*			Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free(void);
};

END