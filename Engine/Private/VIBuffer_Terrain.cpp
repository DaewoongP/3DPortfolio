#include "..\Public\VIBuffer_Terrain.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(_uint iTerrainSizeX, _uint iTerrainSizeY)
{
	m_iNumVertexBuffers = { 1 };
	m_iStride = { sizeof(VTXPOSNORTEX) };
	m_iNumVertices = { (iTerrainSizeX + 1) * (iTerrainSizeY + 1)};
	m_iIndexStride = { sizeof(_uint) };
	m_iNumIndices = { iTerrainSizeX * iTerrainSizeY * 6 };
	m_eFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// VertexBuffer
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = { m_iStride * m_iNumVertices };
	m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT };
	m_BufferDesc.BindFlags = { D3D11_BIND_VERTEX_BUFFER };
	m_BufferDesc.StructureByteStride = { m_iStride };
	m_BufferDesc.CPUAccessFlags = { 0 };
	m_BufferDesc.MiscFlags = { 0 };

	VTXPOSNORTEX* pVertices = new VTXPOSNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOSNORTEX) * m_iNumVertices);
	
	for (_uint iVertexY = 0; iVertexY < iTerrainSizeY + 1; ++iVertexY)
	{
		for (_uint iVertexX = 0; iVertexX < iTerrainSizeX + 1; ++iVertexX)
		{
			pVertices[iVertexX + ((iTerrainSizeX + 1) * iVertexY)].vPosition =
				_float3((_float)iVertexX, 0.f, (_float)iVertexY);

			pVertices[iVertexX + ((iTerrainSizeX + 1) * iVertexY)].vTexCoord =
				_float2(
					(_float)iVertexX / iTerrainSizeX,
					(_float)iVertexY / iTerrainSizeY);
		}
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;
	Safe_Delete_Array(pVertices);

	//IndexBuffer
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = { m_iIndexStride * m_iNumIndices };
	m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT };
	m_BufferDesc.BindFlags = { D3D11_BIND_INDEX_BUFFER };
	m_BufferDesc.StructureByteStride = { 0 };
	m_BufferDesc.CPUAccessFlags = { 0 };
	m_BufferDesc.MiscFlags = { 0 };

	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	_uint		iNumIndices = { 0 };

	for (_uint i = 0; i < iTerrainSizeY; ++i)
	{
		for (_uint j = 0; j < iTerrainSizeX; j++)
		{
			_uint		iIndex = i * (iTerrainSizeX + 1) + j;

			_uint		iIndices[4] = {
				iIndex + iTerrainSizeX + 1,
				iIndex + iTerrainSizeX + 2,
				iIndex + 1,
				iIndex
			};

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[1];
			pIndices[iNumIndices++] = iIndices[2];

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[2];
			pIndices[iNumIndices++] = iIndices[3];
		}
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;
	Safe_Delete_Array(pIndices);

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CVIBuffer_Terrain::RemakeTerrain(_uint iTerrainSizeX, _uint iTerrainSizeY)
{
	Safe_Release(m_pVB);
	Safe_Release(m_pIB);
	if (FAILED(Initialize_Prototype(iTerrainSizeX, iTerrainSizeY)))
		return E_FAIL;
	return S_OK;
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iTerrainSizeX, _uint iTerrainSizeY)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iTerrainSizeX, iTerrainSizeY)))
	{
		MSG_BOX("Failed to Created CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_Terrain::Clone(void* pArg)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Terrain::Free()
{
	__super::Free();
}
