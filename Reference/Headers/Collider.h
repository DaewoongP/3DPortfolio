#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
public:
	enum TYPE { TYPE_SPHERE, TYPE_AABB, TYPE_OBB, TYPE_END };
	
private:
	explicit CCollider(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	TYPE Get_ColliderType() const { return m_eColliderType; }
	_float3 Get_BoundingCenterPosition() const;
	void Set_BoundingDesc(void* pBoundingDesc);
#ifdef _DEBUG
	void Set_Color(_fvector vColor) { XMStoreFloat4(&m_vColor, vColor); }
#endif // _DEBUG

public:
	virtual HRESULT Initialize_Prototype(TYPE eColliderType);
	virtual HRESULT Initialize(class CBounding* pBounding, void* pArg);
	virtual void	Tick(_fmatrix TransformMatrix);

#ifdef _DEBUG
public:
	virtual HRESULT Render() override;
#endif // _DEBUG

public:
	_bool RayIntersects(_fvector vOrigin, _fvector vDirection, _Inout_ _float& fDist);
	_bool Intersects(CCollider* pOtherCollider, _float3* pCollisionBox);
	void OnCollision(COLLISIONDESC::COLTYPE eCollisionType, COLLISIONDESC::COLDIR eCollisionDirection, CCollider* pOtherCollider);
	_bool IsCollision(CCollider* pOtherCollider);
	void ExitCollision(CCollider* pOtherCollider);
	void DeadCollision();

private:
	class CBounding*			m_pBounding = { nullptr };
	TYPE						m_eColliderType = { TYPE_END };

private:
	vector<COLLISIONDESC>		m_Collisions;
	vector<_bool>				m_isDead;

#ifdef _DEBUG
private:
	_float4						m_vColor;
#endif // _DEBUG

public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eColliderType);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END