#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
protected:
	explicit CVIBuffer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer() = default;

public:
	_uint Get_NumVertices() const { return m_iNumVertices; }
	_uint Get_NumIndices() const { return m_iNumIndices; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	
	virtual HRESULT Render();

protected:
	ID3D11Buffer* m_pVB = { nullptr };
	ID3D11Buffer* m_pIB = { nullptr };

protected:
	D3D11_BUFFER_DESC			m_BufferDesc;
	D3D11_SUBRESOURCE_DATA		m_SubResourceData;
	_uint						m_iNumVertices = { 0 };
	_uint						m_iStride = { 0 };
	_uint						m_iNumVertexBuffers = { 0 };
	DXGI_FORMAT					m_eFormat;
	D3D11_PRIMITIVE_TOPOLOGY	m_eTopology;

	_uint						m_iIndexStride = { 0 };
	_uint						m_iNumIndices = { 0 };

protected:
	HRESULT Create_Buffer(ID3D11Buffer** ppOut);

public:
	virtual CComponent* Clone(void* pArg) PURE;
	virtual void Free() override;
};

END