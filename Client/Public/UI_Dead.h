#pragma once
#include "UI.h"
#include "Client_Defines.h"

BEGIN(Client)

class CUI_Dead final : public CUI
{
private:
	explicit CUI_Dead(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Dead(const CUI_Dead& rhs);
	virtual ~CUI_Dead() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iLevelIndex) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual GAMEEVENT Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CPlayer*			m_pPlayer = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CUI_Dead* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END