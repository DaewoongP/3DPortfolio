#pragma once
#include "UI.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)

class CMainSlot final : public CUI
{
private:
	explicit CMainSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMainSlot(const CMainSlot& rhs);
	virtual ~CMainSlot() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iLevelIndex) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual GAMEEVENT Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CPlayer*		m_pPlayer = { nullptr };

private:
	CTexture*		m_pBlinkTexture = { nullptr };
	CTexture*		m_pSurgeTexture = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CMainSlot* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END