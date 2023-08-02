#pragma once
#include "Trigger.h"
#include "Client_Defines.h"

BEGIN(Client)

class CLevelChange_Trigger final : public CTrigger
{
private:
	explicit CLevelChange_Trigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CLevelChange_Trigger(const CLevelChange_Trigger& rhs);
	virtual ~CLevelChange_Trigger() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual GAMEEVENT Late_Tick(_double dTimeDelta) override;
	virtual void OnCollisionEnter(COLLISIONDESC CollisionDesc) override;
	virtual void OnCollisionStay(COLLISIONDESC CollisionDesc) override;
	virtual void OnCollisionExit(COLLISIONDESC CollisionDesc) override;
	virtual HRESULT Render() override;
	virtual HRESULT Reset() override;

public:
	static CLevelChange_Trigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END