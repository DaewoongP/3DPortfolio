#pragma once
#include "UI.h"
#include "Client_Defines.h"
#include "MiniGame_Manager.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)

class CMiniGame_Score final : public CUI
{
private:
	explicit CMiniGame_Score(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMiniGame_Score(const CMiniGame_Score& rhs);
	virtual ~CMiniGame_Score() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual GAMEEVENT Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CMiniGame_Manager*			m_pMiniGame_Manager = { nullptr };
	_uint						m_iOnes = { 0 };
	_uint						m_iTens = { 0 };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CMiniGame_Score* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END