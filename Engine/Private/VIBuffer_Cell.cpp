#include "..\Public\VIBuffer_Cell.h"
#include "Cell.h"
#include "Bounding_Sphere.h"

CVIBuffer_Cell::CVIBuffer_Cell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Cell::CVIBuffer_Cell(const CVIBuffer_Cell& rhs)
	: CVIBuffer(rhs)
	, m_VertexPositions(rhs.m_VertexPositions)
	, m_BoundingSpheres(rhs.m_BoundingSpheres)
{
	for (auto& BoundingSphere : m_BoundingSpheres)
	{
		Safe_AddRef(BoundingSphere);
	}
}

HRESULT CVIBuffer_Cell::Initialize_Prototype(_float3* pPoints)
{
	if (nullptr == pPoints)
		return E_FAIL;

	m_iNumVertexBuffers = { 1 };
	m_iStride = { sizeof(VTXPOS) };
	m_iNumVertices = { 3 };
	m_iIndexStride = { sizeof(_ushort) };
	m_iNumIndices = { 4 };
	m_eFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;


#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	
	m_BufferDesc.ByteWidth = { m_iStride * m_iNumVertices };
	m_BufferDesc.Usage = { D3D11_USAGE_DYNAMIC };
	m_BufferDesc.BindFlags = { D3D11_BIND_VERTEX_BUFFER };
	m_BufferDesc.StructureByteStride = { m_iStride };
	m_BufferDesc.CPUAccessFlags = { D3D11_CPU_ACCESS_WRITE };
	m_BufferDesc.MiscFlags = { 0 };

	VTXPOS* pVertices = new VTXPOS[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOS) * m_iNumVertices);

	m_pOriginSphere = CBounding_Sphere::Create(m_pDevice, m_pContext);
	CBounding_Sphere::BOUNDINGSPHEREDESC	BoundingSphereDesc;
	BoundingSphereDesc.fRadius = 0.5f;
	BoundingSphereDesc.vPosition = _float3(0.f, 0.f, 0.f);

	for (_uint i = 0; i < CCell::POINT_END; ++i)
	{
		pVertices[i].vPosition = pPoints[i];
		m_VertexPositions.push_back(pVertices[i].vPosition);
		m_BoundingSpheres.push_back(static_cast<CBounding_Sphere*>(m_pOriginSphere->Clone(&BoundingSphereDesc)));
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
	m_BufferDesc.Usage = { D3D11_USAGE_DYNAMIC };
	m_BufferDesc.BindFlags = { D3D11_BIND_INDEX_BUFFER };
	m_BufferDesc.StructureByteStride = { 0 };
	m_BufferDesc.CPUAccessFlags = { D3D11_CPU_ACCESS_WRITE };
	m_BufferDesc.MiscFlags = { 0 };

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	// Line Strip
	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;
	pIndices[3] = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Cell::Initialize(void* pArg)
{
	return S_OK;
}

void CVIBuffer_Cell::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	for (_uint i = 0; i < CCell::POINT_END; ++i)
	{
		m_BoundingSpheres[i]->Tick(XMMatrixTranslation(m_VertexPositions[i].x, m_VertexPositions[i].y, m_VertexPositions[i].z));
	}
}

HRESULT CVIBuffer_Cell::Render_Sphere()
{
	for (auto& pSphere : m_BoundingSpheres)
	{
		pSphere->Render();
	}
	return S_OK;
}


HRESULT CVIBuffer_Cell::Begin(_float3* pPoints)
{
	D3D11_MAPPED_SUBRESOURCE MappedSubResource;
	
	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);

	VTXPOS* pVertices = static_cast<VTXPOS*>(MappedSubResource.pData);

	m_VertexPositions.clear();

	for (_uint i = 0; i < CCell::POINT_END; ++i)
	{
		pVertices[i].vPosition = pPoints[i];
		m_VertexPositions.push_back(pVertices[i].vPosition);
	}

	memcpy(MappedSubResource.pData, pVertices, sizeof(VTXPOS) * m_iNumVertices);
	
	return S_OK;
}

HRESULT CVIBuffer_Cell::End()
{
	m_pContext->Unmap(m_pVB, 0);

	return S_OK;
}

CVIBuffer_Cell* CVIBuffer_Cell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _float3* pPoints)
{
	CVIBuffer_Cell* pInstance = new CVIBuffer_Cell(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pPoints)))
	{
		MSG_BOX("Failed to Created CVIBuffer_Cell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Cell::Clone(void* pArg)
{
	CVIBuffer_Cell* pInstance = new CVIBuffer_Cell(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CVIBuffer_Cell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Cell::Free()
{
	__super::Free();

	for (auto& BoundingSphere : m_BoundingSpheres)
	{
		Safe_Release(BoundingSphere);
	}
	m_BoundingSpheres.clear();

	if (false == m_isCloned)
		Safe_Release(m_pOriginSphere);
}
