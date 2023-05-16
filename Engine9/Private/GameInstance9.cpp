#include "GameInstance9.h"
#include "Graphic_Device9.h"
IMPLEMENT_SINGLETON(CGameInstance9)


CGameInstance9::CGameInstance9()
	:m_pGraphic_Device{ CGraphic_Device9::GetInstance() }
{
}

HRESULT CGameInstance9::Initialize_Engine()
{
	return S_OK;
}

void CGameInstance9::Tick_Engine(_double dTimeDelta)
{
}

HRESULT CGameInstance9::Ready_Graphic_Device(HWND hWnd, WINMODE eMode, const _uint& iSizeX, const _uint& iSizeY, _Inout_ LPDIRECT3DDEVICE9* ppDevice)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Ready_Graphic_Device(hWnd, eMode, iSizeX, iSizeY, ppDevice);
}

void CGameInstance9::Render_Begin(D3DXCOLOR Color)
{
	if (nullptr == m_pGraphic_Device)
		return;

	m_pGraphic_Device->Render_Begin(Color);
}

void CGameInstance9::Render_End(void)
{
	if (nullptr == m_pGraphic_Device)
		return;

	m_pGraphic_Device->Render_End();
}

HRESULT CGameInstance9::Add_Timer(const _tchar* pTimerTag)
{
	return S_OK;
}

void CGameInstance9::Tick_Timer(const _tchar* pTimerTag)
{
}

_double CGameInstance9::Get_TimeDelta(const _tchar* pTimerTag)
{
	return _double();
}

void CGameInstance9::Release_Engine()
{
	CGraphic_Device9::GetInstance()->DestroyInstance();
}

void CGameInstance9::Free()
{
	Safe_Release(m_pGraphic_Device);
}
