#include "..\Public\VIBuffer_Rect_Dynamic.h"
#include "Texture.h"

CVIBuffer_Rect_Dynamic::CVIBuffer_Rect_Dynamic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Rect_Dynamic::CVIBuffer_Rect_Dynamic(const CVIBuffer_Rect_Dynamic& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Rect_Dynamic::Initialize_Prototype()
{
	m_iNumVertexBuffers = { 1 };
	m_iStride = { sizeof(VTXPOSTEX) };
	m_iNumVertices = { 4 };
	m_iIndexStride = { sizeof(_ushort) };
	m_iNumIndices = { 6 };
	m_eFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = { m_iStride * m_iNumVertices };
	m_BufferDesc.Usage = { D3D11_USAGE_DYNAMIC };
	m_BufferDesc.BindFlags = { D3D11_BIND_VERTEX_BUFFER };
	m_BufferDesc.StructureByteStride = { m_iStride };
	m_BufferDesc.CPUAccessFlags = { D3D11_CPU_ACCESS_WRITE };
	m_BufferDesc.MiscFlags = { 0 };

	VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

	pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
	pVertices[0].vTexCoord = _float2(0.f, 0.f);

	pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
	pVertices[1].vTexCoord = _float2(1.f, 0.f);

	pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
	pVertices[2].vTexCoord = _float2(1.f, 1.f);

	pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
	pVertices[3].vTexCoord = _float2(0.f, 1.f);

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;
	Safe_Delete_Array(pVertices);

#pragma endregion


#pragma region INDEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = { m_iIndexStride * m_iNumIndices };
	m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT };
	m_BufferDesc.BindFlags = { D3D11_BIND_INDEX_BUFFER };
	m_BufferDesc.StructureByteStride = { 0 };
	m_BufferDesc.CPUAccessFlags = { 0 };
	m_BufferDesc.MiscFlags = { 0 };

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;

	pIndices[3] = 0;
	pIndices[4] = 2;
	pIndices[5] = 3;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;
	Safe_Delete_Array(pIndices);
#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Rect_Dynamic::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CVIBuffer_Rect_Dynamic::Change_TexCoord(const _float& fLeft, const _float& fTop, const _float& fSizeX, const _float& fSizeY, class CTexture* pTexture, _uint iTextureIndex)
{
	if (nullptr == pTexture)
		return E_FAIL;

	_float2 vTextureSize = _float2(0.f, 0.f);

	vTextureSize = pTexture->Get_TextureSize(iTextureIndex);

	D3D11_MAPPED_SUBRESOURCE	MappedSubResource;

	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);

	VTXPOSTEX* pData = static_cast<VTXPOSTEX*>(MappedSubResource.pData);

	pData[0].vTexCoord = _float2(
		fLeft / vTextureSize.x, 
		fTop / vTextureSize.y);

	pData[1].vTexCoord = _float2(
		(fLeft + fSizeX) / vTextureSize.x, 
		fTop / vTextureSize.y);

	pData[2].vTexCoord = _float2(
		(fLeft + fSizeX) / vTextureSize.x,
		(fTop + fSizeY) / vTextureSize.y);

	pData[3].vTexCoord = _float2(
		fLeft / vTextureSize.x,
		(fTop + fSizeY) / vTextureSize.y);

	m_pContext->Unmap(m_pVB, 0);

	return S_OK;
}

CVIBuffer_Rect_Dynamic* CVIBuffer_Rect_Dynamic::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Rect_Dynamic* pInstance = new CVIBuffer_Rect_Dynamic(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CVIBuffer_Rect_Dynamic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Rect_Dynamic::Clone(void* pArg)
{
	CVIBuffer_Rect_Dynamic* pInstance = new CVIBuffer_Rect_Dynamic(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CVIBuffer_Rect_Dynamic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Rect_Dynamic::Free()
{
	__super::Free();

}
