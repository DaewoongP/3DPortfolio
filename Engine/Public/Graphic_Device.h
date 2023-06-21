#pragma once
#include "Base.h"

BEGIN(Engine)
class CGraphic_Device final : public CBase
{
	DECLARE_SINGLETON(CGraphic_Device)
public:
	explicit CGraphic_Device();
	virtual ~CGraphic_Device() = default;

public:
	// Initialize Graphic_Device
	HRESULT Ready_Graphic_Device(HWND hWnd, GRAPHICDESC::WINMODE eWinMode,
		_uint iWinCX, _uint iWinCY, _Inout_ ID3D11Device** ppDevice,
		_Inout_ ID3D11DeviceContext** ppDeviceContextOut);
	// Clear Back Buffer
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	// Clear Depth&Stencil Buffer
	HRESULT Clear_DepthStencil_View();
	// SwapChain's Present Function
	HRESULT Present();

public: /* For. ImGuiRenderTaget */
	// 렌더타겟 초기화 함수
	HRESULT ResetRenderTargets();
	// 버퍼 초기화
	HRESULT Resize_Buffer(_uint& ResizeWidth, _uint& ResizeHeight);

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pDeviceContext = { nullptr };
	IDXGISwapChain*				m_pSwapChain = { nullptr };
	ID3D11RenderTargetView*		m_pBackBufferRTV = { nullptr };
	ID3D11DepthStencilView*		m_pDepthStencilView = { nullptr };

private:
	HRESULT Ready_SwapChain(HWND hWnd, GRAPHICDESC::WINMODE eWinMode, _uint iWinCX, _uint iWinCY);
	HRESULT Ready_BackBufferRenderTargetView();
	HRESULT Ready_DepthStencilRenderTargetView(_uint iWinCX, _uint iWinCY);

public:
	virtual void Free() override;
};

END