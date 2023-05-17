#pragma once

#include "Base.h"

BEGIN(Engine9)

class CGraphic_Device9 : public CBase
{
	DECLARE_SINGLETON(CGraphic_Device9)

private:
	explicit CGraphic_Device9();
	virtual ~CGraphic_Device9() = default;

public:
	HRESULT		Ready_Graphic_Device(HWND hWnd, WINMODE eMode, const _uint& iSizeX, const _uint& iSizeY, _Inout_ LPDIRECT3DDEVICE9* ppDevice);

	void		Render_Begin(D3DXCOLOR Color);
	void		Render_End(void);

private:
	LPDIRECT3D9			m_pSDK = { nullptr };
	LPDIRECT3DDEVICE9	m_pDevice = { nullptr };

private:
	virtual void	Free(void) override;
};

END
