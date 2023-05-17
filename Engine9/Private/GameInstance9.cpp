#include "GameInstance9.h"
#include "Graphic_Device9.h"
#include "Object_Manager.h"
#include "Component_Manager.h"
#include "Timer_Manager.h"

IMPLEMENT_SINGLETON(CGameInstance9)

CGameInstance9::CGameInstance9()
	: m_pGraphic_Device{ CGraphic_Device9::GetInstance() }
	, m_pObject_Manager{ CObject_Manager::GetInstance() }
	, m_pComponent_Manager{ CComponent_Manager::GetInstance() }
	, m_pTimer_Manager{ CTimer_Manager::GetInstance() }
{
	Safe_AddRef(m_pTimer_Manager);
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pComponent_Manager);
}

void CGameInstance9::Tick_Engine(_double dTimeDelta)
{
	if (nullptr == m_pObject_Manager)
		return;

	m_pObject_Manager->Tick(dTimeDelta);
	m_pObject_Manager->Late_Tick(dTimeDelta);
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
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;
	
	return m_pTimer_Manager->Add_Timer(pTimerTag);
}

void CGameInstance9::Tick_Timer(const _tchar* pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return;

	m_pTimer_Manager->Tick_Timer(pTimerTag);
}

_double CGameInstance9::Get_TimeDelta(const _tchar* pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.0;

	return m_pTimer_Manager->Get_TimeDelta(pTimerTag);
}

HRESULT CGameInstance9::Add_Object_Prototype(const _tchar* pPrototypeTag, CGameObject* pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(pPrototypeTag, pPrototype);
}

HRESULT CGameInstance9::Add_GameObject(const _tchar* pPrototypeTag, const _tchar* pObjectTag, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_GameObject(pPrototypeTag, pObjectTag, pArg);
}

HRESULT CGameInstance9::Add_Component_Prototype(const _tchar* pPrototypeTag, CComponent* pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(pPrototypeTag, pPrototype);
}

CComponent* CGameInstance9::Clone_Component(const _tchar* pPrototypeTag, void* pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(pPrototypeTag, pArg);
}

void CGameInstance9::Release_Engine()
{
	CTimer_Manager::GetInstance()->DestroyInstance();
	CComponent_Manager::GetInstance()->DestroyInstance();
	CObject_Manager::GetInstance()->DestroyInstance();
	CGraphic_Device9::GetInstance()->DestroyInstance();
}

void CGameInstance9::Free()
{
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pGraphic_Device);
}
