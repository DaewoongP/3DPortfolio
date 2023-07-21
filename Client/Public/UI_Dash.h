#pragma once
#include "UI.h"

BEGIN(Client)

class CUI_Dash final : public CUI
{
private:
	explicit CUI_Dash(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Dash(const CUI_Dash& rhs);
	virtual ~CUI_Dash() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual GAMEEVENT Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool				m_isDash = { false };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CUI_Dash* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END