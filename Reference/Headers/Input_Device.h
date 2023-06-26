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
	_bool		Get_DIKeyState(_ubyte ubyKeyID, KEYSTATE eState = KEY_PRESSING);
	_bool		Get_DIMouseState(MOUSEKEYSTATE eMouseID, KEYSTATE eState = KEY_PRESSING);
	_long		Get_DIMouseMove(MOUSEMOVESTATE eMouseMoveID);

public:
	HRESULT		Ready_Input_Device(HINSTANCE hInst, HWND hWnd);
	void		Tick(void);

private:
	_bool    Key_Down(const _ubyte& ubyKey);
	_bool    Key_Pressing(const _ubyte& ubyKey);
	_bool    Key_Up(const _ubyte& ubyKey);

	_bool    Mouse_Down(MOUSEKEYSTATE eMouseID);
	_bool    Mouse_Pressing(MOUSEKEYSTATE eMouseID);
	_bool    Mouse_Up(MOUSEKEYSTATE eMouseID);

private:
	// 인풋 객체 생성용 컴객체
	LPDIRECTINPUT8				m_pInputSDK;

	LPDIRECTINPUTDEVICE8		m_pKeyBoard;
	LPDIRECTINPUTDEVICE8		m_pMouse;

private:
	// 현재 키보드 상태값
	_byte                   m_byKeyState[MAX_DIK];
	// 이전 키보드 상태값
	_byte                   m_byPreKeyState[MAX_DIK];
	// 현재 마우스 상태값
	DIMOUSESTATE            m_MouseState;
	// 이전 마우스 상태값
	DIMOUSESTATE            m_PreMouseState;

public:
	virtual void Free();
};

END