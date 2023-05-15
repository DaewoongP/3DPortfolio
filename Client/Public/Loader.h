#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CLoader final : public CBase
{
private:
	explicit CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader() = default;

public:
	const _tchar* Get_LoadingText() const { return m_szLoading; }

	_bool Get_Finished() const { return m_isFinished; }

public:
	HRESULT Initialize(LEVELID eNextLevel);
	HRESULT Loading();

private:
	HANDLE					m_hThread = { 0 };
	CRITICAL_SECTION		m_Critical_Section;

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	CGameInstance*			m_pGameInstance = { nullptr };

private:
	LEVELID					m_eNextLevelID = { LEVELID::LEVEL_END };
	_tchar					m_szLoading[MAX_STR] = TEXT("");
	_bool					m_isFinished = { false };

private:
	HRESULT Loading_For_Logo();
	HRESULT Loading_For_GamePlay();

public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevel);
	virtual void Free() override;
};

END