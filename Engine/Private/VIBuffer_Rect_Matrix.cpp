#include "..\Public\VIBuffer_Rect_Matrix.h"

CVIBuffer_Rect_Matrix::CVIBuffer_Rect_Matrix(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Instance(pDevice, pContext)
{
}

CVIBuffer_Rect_Matrix::CVIBuffer_Rect_Matrix(const CVIBuffer_Rect_Matrix& rhs)
	: CVIBuffer_Instance(rhs)
	, m_InstanceDesc(rhs.m_InstanceDesc)
{
}

HRESULT CVIBuffer_Rect_Matrix::Initialize_Prototype(const INSTANCEDESC* pInstanceDesc, _uint iNumInstance)
{
	m_iNumInstance = { iNumInstance }; // 인스턴스의 개수
	if (FAILED(Make_InstanceLogic(pInstanceDesc)))
		return E_FAIL;

	m_iIndexCountPerInstance = { 6 }; // 사각형(인스턴스) 하나 그리는데 필요한 인덱스 수
	m_iNumVertexBuffers = { 2 };
	m_iStride = { sizeof(VTXPOSTEX) };
	m_iNumVertices = { 4 };
	m_iIndexStride = { sizeof(_ushort) };
	m_iNumIndices = { m_iIndexCountPerInstance * m_iNumInstance };
	m_eFormat = { DXGI_FORMAT_R16_UINT };
	m_eTopology = { D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST };


#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = { m_iStride * m_iNumVertices };
	m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT }; // 정점 버퍼 자체를 움직이는 것이 아닌, 인스턴스버퍼에서 행렬의 값을 변경시켜 셰이더에서 처리.
	m_BufferDesc.BindFlags = { D3D11_BIND_VERTEX_BUFFER };
	m_BufferDesc.StructureByteStride = { m_iStride };
	m_BufferDesc.CPUAccessFlags = { 0 };
	m_BufferDesc.MiscFlags = { 0 };


	VTXPOSTEX* pVertices = new VTXPOSTEX[4];
	ZeroMemory(pVertices, sizeof(VTXPOSTEX) * 4);

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

	_uint		iNumIndices = { 0 };

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		pIndices[iNumIndices++] = 0;
		pIndices[iNumIndices++] = 1;
		pIndices[iNumIndices++] = 2;

		pIndices[iNumIndices++] = 0;
		pIndices[iNumIndices++] = 2;
		pIndices[iNumIndices++] = 3;
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;
	Safe_Delete_Array(pIndices);
#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Rect_Matrix::Initialize(void* pArg)
{
	vector<_float4x4> InitializeMatrices;

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		_float4x4 InitMatrix;
		XMStoreFloat4x4(&InitMatrix, XMMatrixIdentity());

		InitializeMatrices.push_back(InitMatrix);
	}

	if (FAILED(__super::Initialize(InitializeMatrices.data())))
		return E_FAIL;

	return S_OK;
}

void CVIBuffer_Rect_Matrix::Tick(_double dTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE	MappedSubResource;

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);


	
	m_pContext->Unmap(m_pVBInstance, 0);
}

HRESULT CVIBuffer_Rect_Matrix::Make_InstanceLogic(const INSTANCEDESC* pInstanceDesc)
{
	return S_OK;
}

CVIBuffer_Rect_Matrix* CVIBuffer_Rect_Matrix::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCEDESC* pInstanceDesc, _uint iNumInstance)
{
	CVIBuffer_Rect_Matrix* pInstance = new CVIBuffer_Rect_Matrix(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pInstanceDesc, iNumInstance)))
	{
		MSG_BOX("Failed to Created CVIBuffer_Rect_Matrix");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Rect_Matrix::Clone(void* pArg)
{
	CVIBuffer_Rect_Matrix* pInstance = new CVIBuffer_Rect_Matrix(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CVIBuffer_Rect_Matrix");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Rect_Matrix::Free()
{
	__super::Free();
}
