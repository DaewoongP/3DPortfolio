#include "..\Public\Bounding_OBB.h"
#include "Bounding_AABB.h"
#include "Bounding_Sphere.h"

CBounding_OBB::CBounding_OBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBounding(pDevice, pContext)
{
}

CBounding_OBB::CBounding_OBB(const CBounding_OBB& rhs)
	: CBounding(rhs)
	, m_pOBB_Original(new DirectX::BoundingOrientedBox(*rhs.m_pOBB_Original))
	, m_pOBB(new DirectX::BoundingOrientedBox(*m_pOBB_Original))
{
}

void CBounding_OBB::Set_BoundingDesc(void* pBoundingDesc)
{
	if (nullptr != pBoundingDesc)
	{
		m_pOBB_Original->Extents = static_cast<BOUNDINGOBBDESC*>(pBoundingDesc)->vExtents;
		m_pOBB_Original->Center = static_cast<BOUNDINGOBBDESC*>(pBoundingDesc)->vPosition;
		XMStoreFloat4(&m_pOBB_Original->Orientation,
			XMQuaternionRotationRollPitchYaw(
				static_cast<BOUNDINGOBBDESC*>(pBoundingDesc)->vRotation.x,
				static_cast<BOUNDINGOBBDESC*>(pBoundingDesc)->vRotation.y,
				static_cast<BOUNDINGOBBDESC*>(pBoundingDesc)->vRotation.z));
	}
}

HRESULT CBounding_OBB::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_pOBB_Original = new DirectX::BoundingOrientedBox(_float3(0.f, 0.f, 0.f), _float3(0.5f, 0.5f, 0.5f), _float4(0.f, 0.f, 0.f, 1.f));

	return S_OK;
}

HRESULT CBounding_OBB::Initialize(void* pBoundingDesc)
{
	Set_BoundingDesc(pBoundingDesc);

	*m_pOBB = *m_pOBB_Original;

	return S_OK;
}

void CBounding_OBB::Tick(_fmatrix WorldMatrix)
{
	// 원본 객체에서 해당 이동행렬을 받아서 처리된 값을 다른 객체에 넘겨 주고
	// 그 다른객체를 통해 그림.
	m_pOBB_Original->Transform(*m_pOBB,WorldMatrix);
}

_bool CBounding_OBB::RayIntersects(_fvector vOrigin, _fvector vDirection, _float& fDist)
{
	return _bool();
}

_bool CBounding_OBB::Intersects(CCollider::TYPE eColliderType, CBounding* pOtherBounding, _float3* pCollisionBox)
{
	switch (eColliderType)
	{
	case Engine::CCollider::TYPE_SPHERE:
		return m_pOBB->Intersects(*static_cast<CBounding_Sphere*>(pOtherBounding)->Get_Bounding());
	case Engine::CCollider::TYPE_AABB:
		return m_pOBB->Intersects(*static_cast<CBounding_AABB*>(pOtherBounding)->Get_Bounding());
	case Engine::CCollider::TYPE_OBB:
		return m_pOBB->Intersects(*static_cast<CBounding_OBB*>(pOtherBounding)->Get_Bounding());
	}

	return false;
}

#ifdef _DEBUG
HRESULT CBounding_OBB::Render(_fvector vColor)
{
	// 부모의 begin과 end에서 directX에서 지원해주는 여러가지 객체를 통해 그린다.
	__super::Begin();

	// 행렬 변환된 값을 가지고 그린다.
	DX::Draw(m_pPrimitiveBatch, *m_pOBB, vColor);

	__super::End();

	return S_OK;
}
#endif // _DEBUG

CBounding_OBB* CBounding_OBB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBounding_OBB* pInstance = new CBounding_OBB(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBounding_OBB");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBounding* CBounding_OBB::Clone(void* pBoundingDesc)
{
	CBounding_OBB* pInstance = new CBounding_OBB(*this);

	if (FAILED(pInstance->Initialize(pBoundingDesc)))
	{
		MSG_BOX("Failed to Cloned CBounding_OBB");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_OBB::Free()
{
	__super::Free();

	Safe_Delete(m_pOBB_Original);
	Safe_Delete(m_pOBB);
}
