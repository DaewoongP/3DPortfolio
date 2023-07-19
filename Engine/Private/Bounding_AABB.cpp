#include "..\Public\Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"

CBounding_AABB::CBounding_AABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBounding(pDevice, pContext)
{
}

CBounding_AABB::CBounding_AABB(const CBounding_AABB& rhs)
	: CBounding(rhs)
	, m_pAABB_Original(new DirectX::BoundingBox(*rhs.m_pAABB_Original))
	, m_pAABB(new DirectX::BoundingBox(*m_pAABB_Original))
{
}

void CBounding_AABB::Set_BoundingDesc(void* pBoundingDesc)
{
	if (nullptr != pBoundingDesc)
	{
		m_pAABB_Original->Extents = static_cast<BOUNDINGAABBDESC*>(pBoundingDesc)->vExtents;
		m_pAABB_Original->Center = static_cast<BOUNDINGAABBDESC*>(pBoundingDesc)->vPosition;

		*m_pAABB = *m_pAABB_Original;
	}
}

HRESULT CBounding_AABB::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_pAABB_Original = new DirectX::BoundingBox(_float3(0.f, 0.f, 0.f), _float3(0.5f, 0.5f, 0.5f));

	return S_OK;
}

HRESULT CBounding_AABB::Initialize(void* pBoundingDesc)
{
	Set_BoundingDesc(pBoundingDesc);

	return S_OK;
}

void CBounding_AABB::Tick(_fmatrix WorldMatrix)
{
	// 원본 객체에서 해당 이동행렬을 받아서 처리된 값을 다른 객체에 넘겨 주고
	// 그 다른객체를 통해 그림.
	m_pAABB_Original->Transform(*m_pAABB, Remove_Rotation(WorldMatrix));
}

_bool CBounding_AABB::RayIntersects(_fvector vOrigin, _fvector vDirection, _float& fDist)
{
	return _bool();
}

_bool CBounding_AABB::Intersects(CCollider::TYPE eColliderType, CBounding* pOtherBounding, _float3* pCollisionBox)
{
	switch (eColliderType)
	{
	case Engine::CCollider::TYPE_SPHERE:
		return m_pAABB->Intersects(*static_cast<CBounding_Sphere*>(pOtherBounding)->Get_Bounding());
	case Engine::CCollider::TYPE_AABB:
		return IntersectAABB(pOtherBounding, pCollisionBox);
	case Engine::CCollider::TYPE_OBB:
		return m_pAABB->Intersects(*static_cast<CBounding_OBB*>(pOtherBounding)->Get_Bounding());
	}

	return false;
}

#ifdef _DEBUG
HRESULT CBounding_AABB::Render(_fvector vColor)
{
	// 부모의 begin과 end에서 directX에서 지원해주는 여러가지 객체를 통해 그린다.
	__super::Begin();

	// 행렬 변환된 값을 가지고 그린다.
	DX::Draw(m_pPrimitiveBatch, *m_pAABB, vColor);

	__super::End();

	return S_OK;
}
#endif // _DEBUG

_matrix CBounding_AABB::Remove_Rotation(_fmatrix TransformMatrix)
{
	_matrix		ResultMatrix = TransformMatrix;

	ResultMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[0]));
	ResultMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[1]));
	ResultMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[2]));

	return ResultMatrix;
}

_bool CBounding_AABB::IntersectAABB(CBounding* pOtherBounding, _float3* pCollisionBox)
{
	const DirectX::BoundingBox* otherAABB = static_cast<CBounding_AABB*>(pOtherBounding)->m_pAABB;
	_vector vCenterLine, vRadius;
	ZEROMEM(&vCenterLine);

	vCenterLine = XMVectorAbs(XMLoadFloat3(&m_pAABB->Center) - XMLoadFloat3(&otherAABB->Center));

	vRadius = XMLoadFloat3(&m_pAABB->Extents) + XMLoadFloat3(&otherAABB->Extents);

	if (true == XMVector3GreaterOrEqual(vRadius, vCenterLine))
	{
		XMStoreFloat3(pCollisionBox, (vRadius - vCenterLine) / vRadius);

		return true;
	}

	return false;
}

CBounding_AABB* CBounding_AABB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBounding_AABB* pInstance = new CBounding_AABB(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBounding_AABB");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBounding* CBounding_AABB::Clone(void* pBoundingDesc)
{
	CBounding_AABB* pInstance = new CBounding_AABB(*this);

	if (FAILED(pInstance->Initialize(pBoundingDesc)))
	{
		MSG_BOX("Failed to Cloned CBounding_AABB");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_AABB::Free()
{
	__super::Free();

	Safe_Delete(m_pAABB_Original);
	Safe_Delete(m_pAABB);
}
