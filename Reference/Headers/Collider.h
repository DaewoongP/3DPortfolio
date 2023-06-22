#pragma once
#include "Component.h"
#include "Bounding_Sphere.h"

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
	virtual HRESULT Initialize_Prototype(TYPE eColliderType);
	virtual HRESULT Initialize(class CBounding* pBounding, void* pArg);
	virtual void	Tick(_fmatrix TransformMatrix);

#ifdef _DEBUG
public:
	virtual HRESULT Render();
#endif // _DEBUG
	
private:
	PrimitiveBatch<DirectX::VertexPositionColor>* m_pPrimitiveBatch = { nullptr };
	DirectX::BasicEffect*		m_pEffect = { nullptr };
	ID3D11InputLayout*			m_pInputLayout = { nullptr };

	class CBounding*			m_pBounding = { nullptr };

public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eColliderType);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END