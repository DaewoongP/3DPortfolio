#include "VIBuffer.h"

CVIBuffer::CVIBuffer(LPDIRECT3DDEVICE9 pDevice)
	: CComponent(pDevice)
	, m_pVB(nullptr)
	, m_pIB(nullptr)
	, m_dwFVF(0)
	, m_dwVtxSize(0)
	, m_dwVtxCnt(0)
	, m_dwTriCnt(0)
	, m_dwIdxSize(0)
{
	ZeroMemory(&m_IdxFmt, sizeof m_IdxFmt);
}

CVIBuffer::CVIBuffer(const CVIBuffer& rhs)
	:CComponent(rhs)
	, m_pVB(rhs.m_pVB)
	, m_pIB(rhs.m_pIB)
	, m_dwFVF(rhs.m_dwFVF)
	, m_dwVtxSize(rhs.m_dwVtxSize)
	, m_dwVtxCnt(rhs.m_dwVtxCnt)
	, m_dwTriCnt(rhs.m_dwTriCnt)
	, m_dwIdxSize(rhs.m_dwIdxSize)
	, m_IdxFmt(rhs.m_IdxFmt)
{
	m_pVB->AddRef();
	m_pIB->AddRef();
}

HRESULT CVIBuffer::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(Create_Buffer(), E_FAIL);
	return S_OK;
}

HRESULT CVIBuffer::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CVIBuffer::Create_Buffer(void)
{
	if (FAILED(m_pDevice->CreateVertexBuffer(m_dwVtxCnt * m_dwVtxSize,  // 정점 버퍼의 크기
		0,						// 버퍼를 만드는 방식(0인 경우 정적 버퍼,  D3DUSAGE_DYNAMIC 동적 버퍼)
		m_dwFVF,				// 정점의 옵션			
		D3DPOOL_MANAGED,
		&m_pVB,					// 생성된 버텍스 버퍼를 저장할 메모리 주소
		nullptr)))
	{
		MSG_BOX("CreateVertexBuffer Failed");
		return E_FAIL;
	}

	if (FAILED(m_pDevice->CreateIndexBuffer(m_dwIdxSize * m_dwTriCnt,
		0,
		m_IdxFmt,
		D3DPOOL_MANAGED,
		&m_pIB,
		nullptr)))
	{
		MSG_BOX("CreateIndexBuffer Failed");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CVIBuffer::Render(void)
{
	m_pDevice->SetStreamSource(0, m_pVB, 0, m_dwVtxSize);
	m_pDevice->SetFVF(m_dwFVF);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_dwVtxCnt, 0, m_dwTriCnt);

	return S_OK;
}

void CVIBuffer::Free(void)
{
	Safe_Release(m_pIB);
	Safe_Release(m_pVB);

	__super::Free();
}
