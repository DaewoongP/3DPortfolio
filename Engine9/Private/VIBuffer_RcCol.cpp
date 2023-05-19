#include "..\Public\VIBuffer_RcCol.h"

CVIBuffer_RcCol::CVIBuffer_RcCol(LPDIRECT3DDEVICE9 pDevice)
	: CVIBuffer(pDevice)
{
}

CVIBuffer_RcCol::CVIBuffer_RcCol(const CVIBuffer_RcCol& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_RcCol::Initialize_Prototype()
{
	m_dwFVF = FVF_COL;
	m_dwVtxSize = sizeof(VTXCOL);
	m_dwVtxCnt = 4;
	m_dwTriCnt = 2;

	m_dwIdxSize = sizeof(INDEX32);
	m_IdxFmt = D3DFMT_INDEX32;

	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	VTXCOL* pVertex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);
	// 버텍스 버퍼 메모리 공간의 접근을 막는 행위, 3번 인자는 버텍스 버퍼에 저장된 버텍스들 중 첫 번째 버텍스의 주소를 반환한다.

	pVertex[0].vPos = { -0.5f, 0.5f, 0.f };
	pVertex[0].dwColor = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);

	pVertex[1].vPos = { 0.5f, 0.5f, 0.f };
	pVertex[1].dwColor = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);

	pVertex[2].vPos = { 0.5f, -0.5f, 0.f };
	pVertex[2].dwColor = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);

	pVertex[3].vPos = { -0.5f, -0.5f, 0.f };
	pVertex[3].dwColor = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);

	m_pVB->Unlock();

	INDEX32* pIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	pIndex[0]._0 = 0;
	pIndex[0]._1 = 1;
	pIndex[0]._2 = 2;

	pIndex[1]._0 = 0;
	pIndex[1]._1 = 2;
	pIndex[1]._2 = 3;

	m_pIB->Unlock();

	return S_OK;
}

HRESULT CVIBuffer_RcCol::Initialize(void* pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(pArg), E_FAIL);
	return S_OK;
}

HRESULT CVIBuffer_RcCol::Render()
{
	__super::Render();
	return S_OK;
}

CVIBuffer_RcCol* CVIBuffer_RcCol::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CVIBuffer_RcCol* pInstance = new CVIBuffer_RcCol(pDevice);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CVIBuffer_RcCol");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_RcCol::Clone(void* pArg)
{
	CVIBuffer_RcCol* pInstance = new CVIBuffer_RcCol(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CVIBuffer_RcCol");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_RcCol::Free(void)
{
	__super::Free();
}
