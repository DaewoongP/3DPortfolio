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
	HRESULT Ready_Prototype_Component_For_Static();
	HRESULT Open_Level(LEVELID eLevelIndex);

private:
	void Render_FPS(_double dTimeDelta);

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

private:
	CGameInstance*			m_pGameInstance = { nullptr };
	CRenderer*				m_pRenderer = { nullptr };

private:
	TCHAR						m_szFPS[MAX_STR];
	_int						m_iFps = { 0 };
	_double						m_dFpsTime = { 0.0 };

public:
	static CMainApp* Create();
	virtual void Free() override;
};

END