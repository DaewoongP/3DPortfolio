#pragma once
#include "UI.h"
#include "Client_Defines.h"

BEGIN(Client)

class CLoading_OuterCycle final : public CUI
{
private:
	explicit CLoading_OuterCycle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CLoading_OuterCycle(const CLoading_OuterCycle& rhs);
	virtual ~CLoading_OuterCycle() = default;

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
	static CLoading_OuterCycle* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END