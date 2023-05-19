#pragma once
#include "Component.h"

BEGIN(Engine9)

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
protected:
	explicit CVIBuffer(LPDIRECT3DDEVICE9 pDevice);
	explicit CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual HRESULT Render();

protected:
	LPDIRECT3DVERTEXBUFFER9		m_pVB;
	LPDIRECT3DINDEXBUFFER9		m_pIB;

	_ulong		m_dwVtxSize;
	_ulong		m_dwVtxCnt;
	_ulong		m_dwTriCnt;
	_ulong		m_dwFVF;

	_ulong		m_dwIdxSize;
	D3DFORMAT	m_IdxFmt;

protected:
	HRESULT Create_Buffer();

protected:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free(void);
};

END