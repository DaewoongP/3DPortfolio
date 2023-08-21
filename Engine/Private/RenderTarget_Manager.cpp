#include "..\Public\RenderTarget_Manager.h"
#include "RenderTarget.h"

IMPLEMENT_SINGLETON(CRenderTarget_Manager)

CRenderTarget_Manager::CRenderTarget_Manager()
{

}

HRESULT CRenderTarget_Manager::Add_RenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, const _float4& vClearColor)
{
	if (nullptr != Find_RenderTarget(pTargetTag))
		return E_FAIL;

	CRenderTarget* pRenderTarget = CRenderTarget::Create(pDevice, pContext, iSizeX, iSizeY, eFormat, vClearColor);

	if (nullptr == pRenderTarget)
		return E_FAIL;

	m_RenderTargets.emplace(pTargetTag, pRenderTarget);

	return S_OK;
}

HRESULT CRenderTarget_Manager::Add_MRT(const _tchar* pMRTTag, const _tchar* pTargetTag)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(pTargetTag);

	if (nullptr == pRenderTarget)
		return E_FAIL;

	list<CRenderTarget*>* pMRTList = Find_MRT(pMRTTag);

	if (nullptr == pMRTList)
	{
		list<CRenderTarget*>		MRTList;

		MRTList.push_back(pRenderTarget);

		m_MRTs.emplace(pMRTTag, MRTList);
	}
	else
		pMRTList->push_back(pRenderTarget);

	Safe_AddRef(pRenderTarget);

	return S_OK;
}

HRESULT CRenderTarget_Manager::Begin_MRT(ID3D11DeviceContext* pContext, const _tchar* pMRTTag)
{
	list<CRenderTarget*>* pMRTList = Find_MRT(pMRTTag);

	if (nullptr == pMRTList)
		return E_FAIL;

	pContext->OMGetRenderTargets(1, &m_pPreRenderTargetView, &m_pDepthStencilView);

	ID3D11RenderTargetView* pRenderTargets[8] = { nullptr };

	_uint		iNumViews = 0;

	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Clear();
		pRenderTargets[iNumViews++] = pRenderTarget->Get_RTV();
	}

	pContext->OMSetRenderTargets(iNumViews, pRenderTargets, m_pDepthStencilView);

	return S_OK;
}

HRESULT CRenderTarget_Manager::Begin_PreMRT(ID3D11DeviceContext* pContext, const _tchar* pMRTTag)
{
	list<CRenderTarget*>* pMRTList = Find_MRT(pMRTTag);

	if (nullptr == pMRTList)
		return E_FAIL;

	pContext->OMGetRenderTargets(1, &m_pBackBufferView, &m_pDepthStencilView);

	ID3D11RenderTargetView* pRenderTargets[8] = { nullptr };

	_uint		iNumViews = 0;

	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Clear();
		pRenderTargets[iNumViews++] = pRenderTarget->Get_RTV();
	}

	pContext->OMSetRenderTargets(iNumViews, pRenderTargets, m_pDepthStencilView);

	return S_OK;
}

HRESULT CRenderTarget_Manager::End_MRT(ID3D11DeviceContext* pContext)
{
	ID3D11RenderTargetView* pRenderTargets[8] = { m_pPreRenderTargetView };

	pContext->OMSetRenderTargets(8, pRenderTargets, m_pDepthStencilView);

	Safe_Release(m_pPreRenderTargetView);
	Safe_Release(m_pDepthStencilView);

	return S_OK;
}

HRESULT CRenderTarget_Manager::End_PreMRT(ID3D11DeviceContext* pContext)
{
	ID3D11RenderTargetView* pRenderTargets[8] = { m_pBackBufferView };

	pContext->OMSetRenderTargets(8, pRenderTargets, m_pDepthStencilView);

	Safe_Release(m_pBackBufferView);
	Safe_Release(m_pDepthStencilView);

	return S_OK;
}

HRESULT CRenderTarget_Manager::Bind_ShaderResourceView(const _tchar* pTargetTag, CShader* pShader, const _char* pConstantName)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(pTargetTag);

	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Bind_ShaderResourceView(pShader, pConstantName);
}

#ifdef _DEBUG
HRESULT CRenderTarget_Manager::Ready_Debug(const _tchar* pTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(pTargetTag);

	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Ready_Debug(fX, fY, fSizeX, fSizeY);
}

HRESULT CRenderTarget_Manager::Render_Debug(const _tchar* pMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	for (auto& Pair : m_MRTs)
	{
		for (auto& pTarget : Pair.second)
		{
			if (nullptr != pTarget)
				pTarget->Render(pShader, pVIBuffer);
		}
	}

	return S_OK;
}
#endif // _DEBUG

CRenderTarget* CRenderTarget_Manager::Find_RenderTarget(const _tchar* pTargetTag)
{
	auto	iter = find_if(m_RenderTargets.begin(), m_RenderTargets.end(), CTag_Finder(pTargetTag));

	if (iter == m_RenderTargets.end())
		return nullptr;

	return iter->second;
}

list<class CRenderTarget*>* CRenderTarget_Manager::Find_MRT(const _tchar* pMRTTag)
{
	auto	iter = find_if(m_MRTs.begin(), m_MRTs.end(), CTag_Finder(pMRTTag));

	if (iter == m_MRTs.end())
		return nullptr;

	return &iter->second;
}

void CRenderTarget_Manager::Free()
{
	for (auto& Pair : m_MRTs)
	{
		for (auto& pRenderTarget : Pair.second)
			Safe_Release(pRenderTarget);

		Pair.second.clear();
	}

	m_MRTs.clear();

	for (auto& Pair : m_RenderTargets)
		Safe_Release(Pair.second);

	m_RenderTargets.clear();
}
