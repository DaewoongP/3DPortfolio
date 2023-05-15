#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Engine)
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
	HRESULT Open_Level(LEVELID eLevelIndex);

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

private:
	CGameInstance*			m_pGameInstance = { nullptr };

public:
	static CMainApp* Create();
	virtual void Free() override;
};

END