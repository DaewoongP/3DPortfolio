#pragma once
#include "Prop.h"
#include "Bounding_OBB.h"
#include "Bounding_AABB.h"
#include "Bounding_Sphere.h"

BEGIN(Client)

class CColProp : public CProp
{
public:
	typedef struct tagColPropDesc : public tagPropDesc
	{
		CCollider::TYPE				eColliderType;
		CBounding_Sphere::BOUNDINGSPHEREDESC SphereDesc;
		CBounding_AABB::BOUNDINGAABBDESC AABBDesc;
		CBounding_OBB::BOUNDINGOBBDESC OBBDesc;
	}COLPROPDESC;

public:
	CCollider* Get_Collider() { return m_pColliderCom; }

private:
	explicit CColProp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CColProp(const CColProp& rhs);
	virtual ~CColProp() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual GAMEEVENT Late_Tick(_double TimeDelta) override;
	virtual void OnCollisionEnter(COLLISIONDESC CollisionDesc) override;
	virtual void OnCollisionStay(COLLISIONDESC CollisionDesc) override;
	virtual void OnCollisionExit(COLLISIONDESC CollisionDesc) override;
	virtual HRESULT Render() override;

public:
	HRESULT Add_Components(COLPROPDESC ColDesc);
	
private:
	CCollider*		m_pColliderCom = { nullptr };

public:
	static CColProp* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END