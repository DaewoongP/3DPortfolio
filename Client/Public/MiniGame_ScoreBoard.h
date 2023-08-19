#pragma once
#include "UI.h"
#include "Client_Defines.h"
#include "MiniGame_Manager.h"

BEGIN(Client)

class CMiniGame_ScoreBoard final : public CUI
{
private:
	explicit CMiniGame_ScoreBoard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMiniGame_ScoreBoard(const CMiniGame_ScoreBoard& rhs);
	virtual ~CMiniGame_ScoreBoard() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual GAMEEVENT Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CMiniGame_Manager*			m_pMiniGame_Manager = { nullptr };
	_bool						m_isRender = { false };

private:
	_tchar						m_szPerfect[MAX_STR] = TEXT("");
	_tchar						m_szGreat[MAX_STR] = TEXT("");
	_tchar						m_szFailed[MAX_STR] = TEXT("");

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CMiniGame_ScoreBoard* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END