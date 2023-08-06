#include "..\Public\VIBuffer_Color_Instance.h"

CVIBuffer_Color_Instance::CVIBuffer_Color_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Color_Instance::CVIBuffer_Color_Instance(const CVIBuffer_Color_Instance& rhs)
	: CVIBuffer(rhs)
	, m_iInstanceStride(rhs.m_iInstanceStride)
	, m_iIndexCountPerInstance(rhs.m_iIndexCountPerInstance)
	, m_iNumInstance(rhs.m_iNumInstance)
{
}

HRESULT CVIBuffer_Color_Instance::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_Color_Instance::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("Initialize Matrix NULL");
		return E_FAIL;
	}

	vector<_float4x4> InitializeMatrix;
	InitializeMatrix.resize(m_iNumInstance);
	memcpy(InitializeMatrix.data(), pArg, sizeof(_float4x4) * m_iNumInstance);

	D3D11_BUFFER_DESC BufferDesc;
	ZEROMEM(&BufferDesc);

	m_iInstanceStride = sizeof(VTXCOLINSTANCE);
	BufferDesc.ByteWidth = { m_iInstanceStride * m_iNumInstance };
	BufferDesc.Usage = { D3D11_USAGE_DYNAMIC };
	BufferDesc.BindFlags = { D3D11_BIND_VERTEX_BUFFER };
	BufferDesc.StructureByteStride = { m_iInstanceStride };
	BufferDesc.CPUAccessFlags = { D3D11_CPU_ACCESS_WRITE };
	BufferDesc.MiscFlags = { 0 };

	VTXCOLINSTANCE* pVertices = new VTXCOLINSTANCE[m_iNumInstance];

	for (size_t i = 0; i < m_iNumInstance; ++i)
	{
		memcpy(&pVertices[i].vRight, InitializeMatrix[i].m[0], sizeof(_float4));
		memcpy(&pVertices[i].vUp, InitializeMatrix[i].m[1], sizeof(_float4));
		memcpy(&pVertices[i].vLook, InitializeMatrix[i].m[2], sizeof(_float4));
		memcpy(&pVertices[i].vTranslation, InitializeMatrix[i].m[3], sizeof(_float4));
		pVertices[i].vColor = _float4(1.f, 1.f, 1.f, 1.f);
	}

	D3D11_SUBRESOURCE_DATA		SubResourceData;
	ZeroMemory(&SubResourceData, sizeof SubResourceData);

	SubResourceData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&BufferDesc, &SubResourceData, &m_pVBInstance)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CVIBuffer_Color_Instance::Render()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	ID3D11Buffer*			pBuffers[] = {
		m_pVB,
		m_pVBInstance,
	};

	_uint					iStrides[] = {
		m_iStride,
		m_iInstanceStride,
	};

	_uint					iOffset[] = {
		0,
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pBuffers, iStrides, iOffset);
	m_pContext->IASetIndexBuffer(m_pIB, m_eFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_eTopology);
	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

void CVIBuffer_Color_Instance::Free()
{
	__super::Free();

	Safe_Release(m_pVBInstance);
}
