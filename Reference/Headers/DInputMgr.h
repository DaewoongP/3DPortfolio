#pragma once
#include "Base.h"

BEGIN(Engine)

class CDInputMgr : public CBase
{
	DECLARE_SINGLETON(CDInputMgr)
private:
	explicit CDInputMgr();
	virtual ~CDInputMgr();

public:
	_byte		Get_DIKeyState(_ubyte ubyKeyID) 
	{
		return m_byKeyState[ubyKeyID];
	}
	_byte		Get_DIMouseState(MOUSEKEYSTATE eMouseID)
	{
		return m_MouseState.rgbButtons[eMouseID];
	}

	_long		Get_DIMouseMove(MOUSEMOVESTATE eMouseMoveID)
	{
		return *(((_long*)&m_MouseState) + eMouseMoveID);
	}


public:
	HRESULT			Ready_DInput(HINSTANCE hInst, HWND hWnd);
	void			Update_DInput(void);

private:
	LPDIRECTINPUT8				m_pInputSDK;

	LPDIRECTINPUTDEVICE8		m_pKeyBoard;
	LPDIRECTINPUTDEVICE8		m_pMouse;

private:
	_byte					m_byKeyState[256];
	 DIMOUSESTATE			m_MouseState;

public:
	virtual void		Free(void);

};

END