#pragma once
#include "UI.h"

BEGIN(Client)

class CMenuSelect final : public CUI
{
private:
	explicit CMenuSelect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMenuSelect(const CMenuSelect& rhs);
	virtual ~CMenuSelect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual GAMEEVENT Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CMenuSelect* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END