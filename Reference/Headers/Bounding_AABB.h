#pragma once
#include "Bounding.h"

BEGIN(Engine)

class ENGINE_DLL CBounding_AABB final : public CBounding
{
public:
	typedef struct tagBoundingAABBDesc : public CBounding::tagBoundingDesc
	{
		_float3		vExtents;

	}BOUNDINGAABBDESC;

private:
	explicit CBounding_AABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBounding_AABB(const CBounding_AABB& rhs);
	virtual ~CBounding_AABB() = default;

public:
	virtual _float3 Get_CenterPosition() const  override { return m_pAABB->Center; }
	virtual void Set_BoundingDesc(void* pBoundingDesc) override;
	void Set_AABB(BOUNDINGAABBDESC AABBDesc) {
		m_pAABB->Center = AABBDesc.vPosition;
		m_pAABB->Extents = AABBDesc.vExtents;
	}

public:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pBoundingDesc);
	virtual void Tick(_fmatrix WorldMatrix) override;

public:
	virtual _bool RayIntersects(_fvector vOrigin, _fvector vDirection, _Inout_ _float& fDist) override;
	virtual _bool Intersects(CBounding* pOtherBox, _float3* pCollisionBox) override;

#ifdef _DEBUG
public:
	virtual HRESULT Render(_fvector vColor = COLLIDERCOLOR) override;
#endif // _DEBUG

private:
	_matrix Remove_Rotation(_fmatrix TransformMatrix);

private:
	// ��� ��ȯ�� ��ø�Ǿ� ó���� �� �ֱ� ������ �ʿ��� ���� ��ü.
	DirectX::BoundingBox* m_pAABB_Original = { nullptr };
	// ��� ��ȯ�� ���������� �ް� �������ϴ� ��ü.
	DirectX::BoundingBox* m_pAABB = { nullptr };

public:
	static CBounding_AABB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CBounding* Clone(void* pBoundingDesc) override;
	virtual void Free() override;
};

END