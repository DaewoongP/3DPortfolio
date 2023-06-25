#pragma once
#include "Component.h"
#include "Bounding_Sphere.h"
#include "Bounding_AABB.h"

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
	_float3 Get_BoundingCenterPosition() const { return m_pBounding->Get_CenterPosition(); }

public:
	virtual HRESULT Initialize_Prototype(TYPE eColliderType);
	virtual HRESULT Initialize(class CBounding* pBounding, void* pArg);
	virtual void	Tick(_fmatrix TransformMatrix);

#ifdef _DEBUG
public:
	virtual HRESULT Render(_fvector vColor = DirectX::Colors::LightGreen);
#endif // _DEBUG
	
private:
	class CBounding*			m_pBounding = { nullptr };

public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eColliderType);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END