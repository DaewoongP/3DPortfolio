#include "..\Public\Navigation.h"
#include "Cell.h"

#ifdef _DEBUG
#include "Shader.h"
#include "PipeLine.h"
#endif

_float CNavigation::Get_CurrentCellY(_fvector vPosition) const
{
	if (0 > m_NaviDesc.iCurrentIndex)
		return 0.f;

	_vector vPointA = m_Cells[m_NaviDesc.iCurrentIndex]->Get_Point(CCell::POINT_A);
	_vector vPointB = m_Cells[m_NaviDesc.iCurrentIndex]->Get_Point(CCell::POINT_B);
	_vector vPointC = m_Cells[m_NaviDesc.iCurrentIndex]->Get_Point(CCell::POINT_C);

	_vector vCurrentPlane = XMPlaneFromPoints(vPointA, vPointB, vPointC);

	// y = (-ax -cz -d) / b
	return (-1.f * XMVectorGetX(vCurrentPlane) * XMVectorGetX(vPosition) +
		-1.f * XMVectorGetZ(vCurrentPlane) * XMVectorGetZ(vPosition) +
		-1.f * XMVectorGetW(vCurrentPlane)) / XMVectorGetY(vCurrentPlane);
}

CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CNavigation::CNavigation(const CNavigation& rhs)
	: CComponent(rhs)
	, m_Cells(rhs.m_Cells)
#ifdef _DEBUG
	, m_pShader(rhs.m_pShader)
#endif
{
	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);

#ifdef _DEBUG
	Safe_AddRef(m_pShader);
#endif
}

HRESULT CNavigation::Initialize_Prototype(const _tchar* pNavigationFilePath)
{
	_ulong		dwByte = { 0 };

	HANDLE		hFile = CreateFile(pNavigationFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_float3		vPoints[CCell::POINT_END];
	_uint iSize = 0;
	ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iSize; ++i)
	{
		CELLFLAG eCellFlag;

		ReadFile(hFile, &eCellFlag, sizeof(CELLFLAG), &dwByte, nullptr);
		
		ReadFile(hFile, vPoints, sizeof(_float3) * CCell::POINT_END, &dwByte, nullptr);

		CCell* pCell = CCell::Create(m_pDevice, m_pContext, eCellFlag, vPoints, (_int)m_Cells.size());
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.push_back(pCell);
	}

	CloseHandle(hFile);

	// ¼¿µéÀ» ºñ±³ÇØ¼­ ÀÌ¿ô¼¿ °ª Ã¤¿ö³Ö±â.
	if (FAILED(SetUp_Neighbors()))
		return E_FAIL;

#ifdef _DEBUG
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Navigation.hlsl"), VTXPOS_DECL::Elements, VTXPOS_DECL::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;
#endif
	return S_OK;
}

HRESULT CNavigation::Initialize(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_NaviDesc, pArg, sizeof(NAVIGATIONDESC));

	m_iInitialIndex = m_NaviDesc.iCurrentIndex;

	return S_OK;
}

HRESULT CNavigation::Reset()
{
	m_NaviDesc.iCurrentIndex = m_iInitialIndex;

	return S_OK;
}

_bool CNavigation::Is_Move(_fvector vPosition, _Inout_ _float3* pNormal, _Inout_ CELLFLAG* pFlag)
{
	_int		iNeighborIndex = -1;
	_vector vXZPosition = XMVectorSet(XMVectorGetX(vPosition), 0.f, XMVectorGetZ(vPosition), 1.f);

	if (true == m_Cells[m_NaviDesc.iCurrentIndex]->Is_In(vXZPosition, pNormal, &iNeighborIndex, pFlag))
	{
		return true;
	}
	else
	{
		if (-1 != iNeighborIndex)
		{
			while (true)
			{
				if (-1 == iNeighborIndex)
					return false;

				if (true == m_Cells[iNeighborIndex]->Is_In(vXZPosition, pNormal, &iNeighborIndex, pFlag))
					break;
			}

			m_NaviDesc.iCurrentIndex = iNeighborIndex;

			return true;
		}
		else
			return false;
	}

	return false;
}

HRESULT CNavigation::Find_MyCell(_fvector vPosition)
{
	for (auto& pCell : m_Cells)
	{
		if (true == pCell->Is_In(vPosition, nullptr, nullptr, nullptr))
			m_iInitialIndex = m_NaviDesc.iCurrentIndex = pCell->Get_CellIndex();
	}

	return S_OK;
}

#ifdef _DEBUG
HRESULT CNavigation::Render()
{
	if (nullptr == m_pShader)
		return E_FAIL;

	_float4x4		WorldMatrix;
	
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	WorldMatrix._42 += 0.2f;

	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", pPipeLine->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pPipeLine->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	Safe_Release(pPipeLine);

	_float4		vColor = -1 == m_NaviDesc.iCurrentIndex ? _float4(0.f, 1.f, 0.f, 1.f) : _float4(1.f, 0.f, 0.f, 1.f);

	if (FAILED(m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
		return E_FAIL;

	if (-1 == m_NaviDesc.iCurrentIndex)
	{
		if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
			return E_FAIL;

		m_pShader->Begin(0);

		vector<CCell*> FallCells, SlideCells;

		for (auto& pCell : m_Cells)
		{
			if (CELL_FALL == pCell->Get_CellFlag())
				FallCells.push_back(pCell);
			else if (CELL_SLIDE == pCell->Get_CellFlag())
				SlideCells.push_back(pCell);
			else
				pCell->Render();
		}
		
		vColor = _float4(1.f, 0.f, 1.f, 1.f);
		// Fall Cells
		if (FAILED(m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
			return E_FAIL;

		m_pShader->Begin(0);

		for (auto& pCell : FallCells)
		{
			pCell->Render();
		}

		FallCells.clear();

		vColor = _float4(1.f, 1.f, 0.f, 1.f);
		// Slide Cells
		if (FAILED(m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
			return E_FAIL;

		m_pShader->Begin(0);

		for (auto& pCell : SlideCells)
		{
			pCell->Render();
		}

		SlideCells.clear();
	}
	else
	{
		WorldMatrix._42 += 0.1f;
		if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
			return E_FAIL;

		m_pShader->Begin(0);

		m_Cells[m_NaviDesc.iCurrentIndex]->Render();
	}

	return S_OK;
}

#endif // _DEBUG

HRESULT CNavigation::SetUp_Neighbors()
{
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
			{
				pSourCell->SetUp_Neighbor(CCell::NEIGHBOR_AB, pDestCell);
			}

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
			{
				pSourCell->SetUp_Neighbor(CCell::NEIGHBOR_BC, pDestCell);
			}

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
			{
				pSourCell->SetUp_Neighbor(CCell::NEIGHBOR_CA, pDestCell);
			}
		}
	}

	return S_OK;
}

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationFilePath)
{
	CNavigation* pInstance = new CNavigation(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(pNavigationFilePath)))
	{
		MSG_BOX("Failed to Created CNavigation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CNavigation::Clone(void* pArg)
{
	CNavigation* pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CNavigation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();
#ifdef _DEBUG
	Safe_Release(m_pShader);
#endif

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	m_Cells.clear();
}
