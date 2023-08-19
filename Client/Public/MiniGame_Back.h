#pragma once
#include "UI.h"
#include "Client_Defines.h"

BEGIN(Client)

class CMiniGame_Back final : public CUI
{
private:
	explicit CMiniGame_Back(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMiniGame_Back(const CMiniGame_Back& rhs);
	virtual ~CMiniGame_Back() = default;

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
	static CMiniGame_Back* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END