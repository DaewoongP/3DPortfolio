#include "..\Public\Collider.h"
#include "Composite.h"
#include "GameObject.h"

CCollider::CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CCollider::CCollider(const CCollider& rhs)
	: CComponent(rhs)
	, m_eColliderType(rhs.m_eColliderType)
{
}

HRESULT CCollider::Initialize_Prototype(TYPE eColliderType)
{
	m_eColliderType = eColliderType;

	switch (eColliderType)
	{
	case TYPE_SPHERE:
		m_pBounding = CBounding_Sphere::Create(m_pDevice, m_pContext);
		break;
	case TYPE_AABB:
		m_pBounding = CBounding_AABB::Create(m_pDevice, m_pContext);
		break;
	case TYPE_OBB:
		m_pBounding = CBounding_OBB::Create(m_pDevice, m_pContext);
		break;
	}

	return S_OK;
}

HRESULT CCollider::Initialize(CBounding* pBounding, void* pArg)
{
	m_pBounding = pBounding->Clone(pArg);
	
	if (nullptr == m_pBounding)
		return E_FAIL;

	return S_OK;
}

void CCollider::Tick(_fmatrix TransformMatrix)
{
	if (nullptr == m_pBounding)
		return;

	m_pBounding->Tick(TransformMatrix);
}

#ifdef _DEBUG
HRESULT CCollider::Render(_fvector vColor)
{
	if (nullptr == m_pBounding)
		return E_FAIL;

	m_pBounding->Render(vColor);

	return S_OK;
}
#endif // _DEBUG

_bool CCollider::Intersects(CCollider* pOtherCollider, _float3* pCollisionBox)
{
	return m_pBounding->Intersects(pOtherCollider->m_pBounding, pCollisionBox);
}

void CCollider::OnCollision(COLLISIONDESC::COLDIR eCollisionDirection, CCollider* pOtherCollider)
{
	if (nullptr == m_pOwner ||
		nullptr == pOtherCollider)
		return;

	COLLISIONDESC CollisionDesc;

	// 이전에 충돌이 없었고, 방금 충돌함 (Enter)
	if (false == IsCollision(pOtherCollider))
	{
		CollisionDesc.ColDir = eCollisionDirection;
		CollisionDesc.pOtherCollider = pOtherCollider;

		m_Collisions.emplace(pOtherCollider, CollisionDesc);

		Safe_AddRef(pOtherCollider);

		static_cast<CGameObject*>(m_pOwner)->OnCollisionEnter(CollisionDesc);
	}
}

_bool CCollider::IsCollision(CCollider* pOtherCollider)
{
	if (nullptr == pOtherCollider)
		return false;

	auto iter = m_Collisions.find(pOtherCollider);

	if (iter == m_Collisions.end())
		return false;

	// 이미 맵안에 들어가있었기때문에 Stay인게 확정.
	// 나머지는 갱신할 필요 없음.

	static_cast<CGameObject*>(m_pOwner)->OnCollisionStay(iter->second);

	return true;
}

void CCollider::ExitCollision(CCollider* pOtherCollider)
{
	// 충돌을 안했는데 안에 있었다면 Exit상태 처리.
	if (nullptr == pOtherCollider)
		return;

	auto iter = m_Collisions.find(pOtherCollider);

	if (iter == m_Collisions.end())
		return;

	static_cast<CGameObject*>(m_pOwner)->OnCollisionExit(iter->second);

	iter->first->Release();

	m_Collisions.erase(iter);
}

CCollider* CCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eColliderType)
{
	CCollider* pInstance = new CCollider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eColliderType)))
	{
		MSG_BOX("Failed to Created CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CCollider::Clone(void* pArg)
{
	CCollider* pInstance = new CCollider(*this);

	if (FAILED(pInstance->Initialize(m_pBounding, pArg)))
	{
		MSG_BOX("Failed to Cloned CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider::Free()
{
	__super::Free();

	Safe_Release(m_pBounding);

	// 키값은 nullptr로 채우는 작업이 불가능함.
	for (auto& iter : m_Collisions)
		iter.first->Release();
	m_Collisions.clear();
}