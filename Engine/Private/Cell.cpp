#include "..\Public\Cell.h"

#ifdef _DEBUG
#include "VIBuffer_Cell.h"
#endif // _DEBUG

CCell::CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	ZEROMEMSIZE(m_vPoints, sizeof(_float3) * POINT_END);
}

HRESULT CCell::Initialize(CELLFLAG eCellFlag, _float3* pPoints, _int iIndex)
{
	memcpy(m_vPoints, pPoints, sizeof(_float3) * POINT_END);

	for (_uint i = 0; i < POINT_END; ++i)
	{
		m_vPoints[i].y = 0;
	}

	m_iIndex = iIndex;
	m_eCellFlag = eCellFlag;

	_vector		vLine;

	// x, z,를 - 붙이면서 스왑하면 수직벡터를 구할 수 있음.
	vLine = (XMLoadFloat3(&pPoints[POINT_B]) - XMLoadFloat3(&pPoints[POINT_A]));
	m_vNormals[NEIGHBOR_AB] = _float3(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine));

	vLine = (XMLoadFloat3(&pPoints[POINT_C]) - XMLoadFloat3(&pPoints[POINT_B]));
	m_vNormals[NEIGHBOR_BC] = _float3(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine));

	vLine = (XMLoadFloat3(&pPoints[POINT_A]) - XMLoadFloat3(&pPoints[POINT_C]));
	m_vNormals[NEIGHBOR_CA] = _float3(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine));

#ifdef _DEBUG
	m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, pPoints);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;
#endif

	return S_OK;
}

_bool CCell::Is_In(_fvector vPosition, _Inout_ _float3* pNormal, _Inout_ _int* pNeighborIndex, _Inout_ CELLFLAG* pFlag)
{
	for (_uint i = 0; i < NEIGHBOR_END; ++i)
	{
		_vector		vDir = XMVector3Normalize(vPosition - XMLoadFloat3(&m_vPoints[i]));
		_vector		vNormal = XMVector3Normalize(XMLoadFloat3(&m_vNormals[i]));

		if (0 < XMVectorGetX(XMVector3Dot(vDir, vNormal)))
		{
			if (nullptr != pNeighborIndex)
				*pNeighborIndex = m_iNeighborIndices[i];

			if (nullptr != pNormal &&
				nullptr != pFlag)
			{
				if (-1 == m_iNeighborIndices[i])
					XMStoreFloat3(pNormal, vNormal);

				*pFlag = m_eCellFlag;
			}
			
			return false;
		}
	}

	if (nullptr != pFlag)
		*pFlag = m_eCellFlag;

	return true;
}

_bool CCell::Compare_Points(_fvector vSourPoint, _fvector vDestPoint)
{
	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), vSourPoint))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), vDestPoint))
		{
			return true;
		}
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), vDestPoint))
		{
			return true;
		}
	}

	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), vSourPoint))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), vDestPoint))
		{
			return true;
		}
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), vDestPoint))
		{
			return true;
		}
	}

	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), vSourPoint))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), vDestPoint))
		{
			return true;
		}
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), vDestPoint))
		{
			return true;
		}
	}

	return false;
}

#ifdef _DEBUG
HRESULT CCell::Render()
{
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	m_pVIBuffer->Render();

	return S_OK;
}
#endif

CCell* CCell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CELLFLAG eCellFlag, _float3* pPoints, _int iIndex)
{
	CCell* pInstance = new CCell(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eCellFlag, pPoints, iIndex)))
	{
		MSG_BOX("Failed to Created CCell");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCell::Free()
{
#ifdef _DEBUG
	Safe_Release(m_pVIBuffer);
#endif

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
