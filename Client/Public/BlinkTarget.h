#pragma once
#include "UI.h"
#include "Client_Defines.h"

BEGIN(Client)

class CBlinkTarget final : public CUI
{
private:
	explicit CBlinkTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBlinkTarget(const CBlinkTarget& rhs);
	virtual ~CBlinkTarget() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iLevelIndex) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual GAMEEVENT Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CPlayer*		m_pPlayer = { nullptr };
	_float4				m_vColor;

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CBlinkTarget* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END