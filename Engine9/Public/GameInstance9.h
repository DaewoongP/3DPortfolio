#pragma once
#include "Base.h"

BEGIN(Engine9)

class ENGINE_DLL CGameInstance9 final : public CBase
{
	DECLARE_SINGLETON(CGameInstance9)

private:
	explicit CGameInstance9();
	virtual ~CGameInstance9() = default;

public:
	void Tick_Engine(_double dTimeDelta);

public: // Graphic_Device
	HRESULT		Ready_Graphic_Device(HWND hWnd, WINMODE eMode, const _uint & iSizeX, const _uint & iSizeY, _Inout_ LPDIRECT3DDEVICE9 * ppDevice);
	void		Render_Begin(D3DXCOLOR Color);
	void		Render_End(void);

public: /* For.Timer_Manager */
	HRESULT		Add_Timer(const _tchar * pTimerTag);
	void		Tick_Timer(const _tchar * pTimerTag);
	_double		Get_TimeDelta(const _tchar * pTimerTag);

public: /* For.Object_Manager */
	HRESULT Add_Object_Prototype(const _tchar * pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObject(const _tchar * pPrototypeTag, const _tchar * pObjectTag, void* pArg = nullptr);

public: /* For.Component_Manager*/
	HRESULT Add_Component_Prototype(const _tchar * pPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(const _tchar * pPrototypeTag, void* pArg = nullptr);

private:
	class CGraphic_Device9*		m_pGraphic_Device = { nullptr };
	class CObject_Manager*		m_pObject_Manager = { nullptr };
	class CComponent_Manager*	m_pComponent_Manager = { nullptr };
	class CTimer_Manager*		m_pTimer_Manager = { nullptr };
public:
	static void Release_Engine();
	virtual void Free() override;
};

END