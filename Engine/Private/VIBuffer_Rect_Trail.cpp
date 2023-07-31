#include "..\Public\VIBuffer_Rect_Trail.h"
#include "Calculator.h"

CVIBuffer_Rect_Trail::CVIBuffer_Rect_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Rect_Trail::CVIBuffer_Rect_Trail(const CVIBuffer_Rect_Trail& rhs)
	: CVIBuffer(rhs)
	, m_iTrailNum(rhs.m_iTrailNum)
	, m_fSizeX(rhs.m_fSizeX)
{
}

HRESULT CVIBuffer_Rect_Trail::Initialize_Prototype(_uint iTrailNum, _float fSizeX)
{
	m_iTrailNum = iTrailNum;
	m_fSizeX = fSizeX;

	m_iNumVertexBuffers = { 1 };
	m_iStride = { sizeof(VTXPOSTEX) };
	m_iNumVertices = { 2 * (m_iTrailNum + 1) };
	m_iIndexStride = { sizeof(_ushort) };
	m_iNumIndices = { 6 * m_iTrailNum };
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

	/*
	9 7 5 3 1
	8 6 4 2 0
	*/

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		// È¦¼ö ... 9 7 5 3 1
		_uint iVertexIndex = { 0 };
		if (1 == i % 2)
		{
			iVertexIndex = (i - 1) >> 1;
			pVertices[i].vPosition = _float3(-1.f * iVertexIndex, 1.f, 0.f);
			pVertices[i].vTexCoord = _float2((_float)iVertexIndex / m_iTrailNum, 1.f);
		}
		// Â¦¼ö ... 8 6 4 2 0
		else
		{
			iVertexIndex = i >> 1;
			pVertices[i].vPosition = _float3(-1.f * iVertexIndex, 0.f, 0.f);
			pVertices[i].vTexCoord = _float2((_float)iVertexIndex / m_iTrailNum, 0.f);
		}
	}

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
	/*
	9 7 5 3 1
	8 6 4 2 0
	*/

	/*
	3 1 0 / 3 0 2 , 5 3 2 / 5 2 4 , 7 5 4 / 7 4 6
	*/
	_uint iIndex = { 0 };
	for (_uint i = 0; i < m_iNumIndices;)
	{
		pIndices[i++] = iIndex + 3;
		pIndices[i++] = iIndex + 1;
		pIndices[i++] = iIndex + 0;

		pIndices[i++] = iIndex + 3;
		pIndices[i++] = iIndex + 0;
		pIndices[i++] = iIndex + 2;

		iIndex += 2;
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;
	Safe_Delete_Array(pIndices);
#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Rect_Trail::Initialize(void* pArg)
{
	return S_OK;
}

void CVIBuffer_Rect_Trail::Tick(_float3 vLocalHighPosition, _float3 vLocalLowPosition, _fmatrix WorldMatrix, _double dTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE	MappedSubResource;

	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);

	VTXPOSTEX* pData = static_cast<VTXPOSTEX*>(MappedSubResource.pData);

	VTXPOSTEX* pCopyData = pData;

	for (_uint i = 2; i < m_iNumVertices; ++i)
	{
		pData[i].vPosition = pCopyData[i - 2].vPosition;
	}

	XMStoreFloat3(&pData[0].vPosition,
		XMVector3TransformCoord(XMLoadFloat3(&vLocalLowPosition), WorldMatrix));
	XMStoreFloat3(&pData[1].vPosition,
		XMVector3TransformCoord(XMLoadFloat3(&vLocalHighPosition), WorldMatrix));

	m_pContext->Unmap(m_pVB, 0);
}

CVIBuffer_Rect_Trail* CVIBuffer_Rect_Trail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iTrailNum, _float fSizeX)
{
	CVIBuffer_Rect_Trail* pInstance = new CVIBuffer_Rect_Trail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iTrailNum, fSizeX)))
	{
		MSG_BOX("Failed to Created CVIBuffer_Rect_Trail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Rect_Trail::Clone(void* pArg)
{
	CVIBuffer_Rect_Trail* pInstance = new CVIBuffer_Rect_Trail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CVIBuffer_Rect_Trail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Rect_Trail::Free()
{
	__super::Free();
}
