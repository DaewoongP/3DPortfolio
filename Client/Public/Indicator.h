#pragma once
#include "UI.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CIndicator final : public CUI
{
private:
	explicit CIndicator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CIndicator(const CIndicator& rhs);
	virtual ~CIndicator() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual GAMEEVENT Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_float m_fIndicatorRange = { 0.f };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CIndicator* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END