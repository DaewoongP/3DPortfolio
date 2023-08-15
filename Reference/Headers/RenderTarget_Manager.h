#pragma once
#include "Base.h"

BEGIN(Engine)

class CRenderTarget_Manager final : public CBase
{
	DECLARE_SINGLETON(CRenderTarget_Manager)
private:
	explicit CRenderTarget_Manager();
	virtual ~CRenderTarget_Manager() = default;

public:
	HRESULT Add_RenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, const _float4& vClearColor);
	HRESULT Add_MRT(const _tchar* pMRTTag, const _tchar* pTargetTag);

	/* pMRTTag에 추가되어있는 렌더타겟들을 장치에 바인딩한다. */
	HRESULT Begin_MRT(ID3D11DeviceContext* pContext, const _tchar* pMRTTag);
	HRESULT Begin_MRT(ID3D11DeviceContext* pContext, const _tchar* pMRTTag, ID3D11DepthStencilView* pDepthStencilView);

	/* 원래상태로 복구한다. (0번째에 백버퍼가 바인딩 된 상태로 돌려준다.) */
	HRESULT End_MRT(ID3D11DeviceContext* pContext);

	HRESULT Bind_ShaderResourceView(const _tchar* pTargetTag, class CShader* pShader, const _char* pConstantName);

#ifdef _DEBUG
public:
	HRESULT Ready_Debug(const _tchar* pTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_Debug(const _tchar* pMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif // _DEBUG

private:
	unordered_map<const _tchar*, class CRenderTarget*>			m_RenderTargets;

	/* 멀티렌더타겟.(동시에 장치에 바인딩되어야할 렌더타겟들을 모아둔다.) */
	unordered_map<const _tchar*, list<class CRenderTarget*>>	m_MRTs;

private:
	ID3D11RenderTargetView* m_pBackBufferView = { nullptr };
	ID3D11DepthStencilView* m_pDepthStencilView = { nullptr };

private:
	class CRenderTarget* Find_RenderTarget(const _tchar* pTargetTag);
	list<class CRenderTarget*>* Find_MRT(const _tchar* pMRTTag);

public:
	virtual void Free() override;
};

END