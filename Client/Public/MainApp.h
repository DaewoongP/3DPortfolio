#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRenderer;
class CGameInstance;
END

BEGIN(Client)

class CMainApp final : public CBase
{
private:
	explicit CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	void Tick(_double dTimeDelta);
	HRESULT Render();

public:
	// 게임 전체에서 사용하기 위한 스태틱 컴포넌트 생성 함수
	HRESULT Ready_Prototype_Component_For_Static();
	HRESULT Ready_Fonts();
	HRESULT Open_Level(LEVELID eLevelIndex);

#ifdef _DEBUG
private:
	void Render_FPS(_double dTimeDelta);
#endif // _DEBUG

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

private:
	CGameInstance*			m_pGameInstance = { nullptr };
	CRenderer*				m_pRenderer = { nullptr };

#ifdef _DEBUG
private: /* For. Frame Per Second*/
	TCHAR						m_szFPS[MAX_STR];
	_int						m_iFps = { 0 };
	_double						m_dFpsTime = { 0.0 };
#endif // _DEBUG

public:
	static CMainApp* Create();
	virtual void Free() override;
};

END