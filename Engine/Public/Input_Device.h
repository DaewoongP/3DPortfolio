#pragma once
#include "Base.h"
#include "Engine_Defines.h"

BEGIN(Engine)

class CInput_Device : public CBase
{
	DECLARE_SINGLETON(CInput_Device)
public:
	enum MOUSEKEYSTATE { DIMK_LBUTTON, DIMK_RBUTTON, DIMK_WHEEL, DIMK_END };
	enum MOUSEMOVESTATE { DIMM_X, DIMM_Y, DIMM_WHEEL, DIMM_END };
	enum KEYSTATE { KEY_DOWN, KEY_PRESSING, KEY_UP, KEY_END };

private:
	explicit CInput_Device();
	virtual ~CInput_Device() = default;

public:
	_byte		Get_DIKeyState(_ubyte ubyKeyID, KEYSTATE eState);
	_byte		Get_DIMouseState(MOUSEKEYSTATE eMouseID, KEYSTATE eState);
	_long		Get_DIMouseMove(MOUSEMOVESTATE eMouseMoveID);

public:
	HRESULT			Ready_Input_Device(HINSTANCE hInst, HWND hWnd);
	void			Tick(void);

private:
	_bool    Key_Down(const _ubyte& ubyKey);
	_bool    Key_Pressing(const _ubyte& ubyKey);
	_bool    Key_Up(const _ubyte& ubyKey);

	_bool    Mouse_Down(MOUSEKEYSTATE eMouseID);
	_bool    Mouse_Pressing(MOUSEKEYSTATE eMouseID);
	_bool    Mouse_Up(MOUSEKEYSTATE eMouseID);

private:
	LPDIRECTINPUT8				m_pInputSDK;

	LPDIRECTINPUTDEVICE8		m_pKeyBoard;
	LPDIRECTINPUTDEVICE8		m_pMouse;

private:
	_byte                   m_byKeyState[MAX_DIK];
	_byte                   m_byPreKeyState[MAX_DIK];
	DIMOUSESTATE            m_MouseState;
	DIMOUSESTATE            m_PreMouseState;

public:
	virtual void		Free(void);

};

END