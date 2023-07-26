#pragma once
#include "UI.h"
#include "Client_Defines.h"

BEGIN(Client)

class CMiniMap final : public CUI
{
private:
	explicit CMiniMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMiniMap(const CMiniMap& rhs);
	virtual ~CMiniMap() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iLevelIndex) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual GAMEEVENT Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CPlayer* m_pPlayer = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CMiniMap* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END