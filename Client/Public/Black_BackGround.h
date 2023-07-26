#pragma once
#include "UI.h"
#include "Client_Defines.h"

BEGIN(Client)

class CBlack_BackGround final : public CUI
{
private:
	explicit CBlack_BackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBlack_BackGround(const CBlack_BackGround& rhs);
	virtual ~CBlack_BackGround() = default;

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
	static CBlack_BackGround* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END