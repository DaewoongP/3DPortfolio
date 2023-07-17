#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CCollider;
class CRenderer;
END

BEGIN(Client)

class CTrigger abstract : public CGameObject
{
protected:
	explicit CTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTrigger(const CTrigger& rhs);
	virtual ~CTrigger() = default;

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

protected:
	CCollider*		m_pColliderCom = { nullptr };
	
protected:
#ifdef _DEBUG
	CRenderer*		m_pRenderer = { nullptr };
	_float4			m_vColliderColor;
#endif // _DEBUG

private:
	HRESULT Add_Component();

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};

END