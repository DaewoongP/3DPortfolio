#pragma once
#include "Base.h"

BEGIN(Engine)

class CRenderTarget final : public CBase
{
private:
	explicit CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderTarget() = default;

public:
	HRESULT Initialize(_uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat);

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

private:
	ID3D11Texture2D*			m_pTexture2D = { nullptr };
	ID3D11RenderTargetView*		m_pRTV = { nullptr };
	ID3D11ShaderResourceView*	m_pSRV = { nullptr };

public:
	static CRenderTarget* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat);
	virtual void Free() override;
};

END