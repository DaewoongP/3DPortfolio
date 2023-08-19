#pragma once
#include "UI.h"
#include "Client_Defines.h"
#include "MiniGame_Manager.h"

BEGIN(Client)

class CMiniGame_Great final : public CUI
{
private:
	explicit CMiniGame_Great(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMiniGame_Great(const CMiniGame_Great& rhs);
	virtual ~CMiniGame_Great() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual GAMEEVENT Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CMiniGame_Manager*		m_pMiniGame_Manager = { nullptr };
	_float					m_fLeftX = { 0.f };
	_float					m_fRightX = { 0.f };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	_float Get_WindowPixelX(_float fPercent);

public:
	static CMiniGame_Great* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END