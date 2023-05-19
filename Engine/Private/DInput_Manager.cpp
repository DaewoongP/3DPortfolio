#include "DInput_Manager.h"

IMPLEMENT_SINGLETON(CDInput_Manager)

CDInput_Manager::CDInput_Manager()
	: m_pInputSDK(nullptr)
	, m_pKeyBoard(nullptr)
	, m_pMouse(nullptr)
{
	ZeroMemory(m_byKeyState, sizeof(m_byKeyState));
	ZeroMemory(&m_MouseState, sizeof(DIMOUSESTATE));
}


CDInput_Manager::~CDInput_Manager()
{
	Free();
}

HRESULT CDInput_Manager::Ready_DInput(HINSTANCE hInst, HWND hWnd)
{
	// 부모 컴객체 생성
	FAILED_CHECK_RETURN(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInputSDK, nullptr), E_FAIL);

	// 내 컴퓨터에 연결되어 있는 키보드 객체 생성
	FAILED_CHECK_RETURN(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr), E_FAIL);

	// 생성된 키보드 객체애 대한 정보를 컴객체에게 전달
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	// 생성된 키보드 객체의 동작 범위를 설정
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 생성된 키보드 컴객체를 활성화
	m_pKeyBoard->Acquire();

	// 내 컴퓨터에 연결되어 있는 마우스 객체 생성
	FAILED_CHECK_RETURN(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr), E_FAIL);

	// 생성된 마우스 객체애 대한 정보를 컴객체에게 전달
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	// 생성된 마우스 객체의 동작 범위를 설정
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 생성된 마우스 컴객체를 활성화
	m_pMouse->Acquire();

	return S_OK;
}

void CDInput_Manager::Update_DInput(void)
{
	m_pKeyBoard->GetDeviceState(256, m_byKeyState);
	m_pMouse->GetDeviceState(sizeof(m_MouseState), &m_MouseState);
}

void CDInput_Manager::Free(void)
{
	Safe_Release(m_pKeyBoard);
	Safe_Release(m_pMouse);
	Safe_Release(m_pInputSDK);
}
