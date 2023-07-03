#include "..\Public\Collider.h"
#include "Composite.h"
#include "GameObject.h"
#include "Collision_Manager.h"

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

_bool CCollider::RayIntersects(_fvector vOrigin, _fvector vDirection, _Inout_ _float& fDist)
{
	return m_pBounding->RayIntersects(vOrigin, vDirection, fDist);
}

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

		m_Collisions.push_back(CollisionDesc);

		static_cast<CGameObject*>(m_pOwner)->OnCollisionEnter(CollisionDesc);
	}
}

_bool CCollider::IsCollision(CCollider* pOtherCollider)
{
	auto iter = find_if(m_Collisions.begin(), m_Collisions.end(), [&](COLLISIONDESC CollDesc)
		{
			if (CollDesc.pOtherCollider == pOtherCollider)
				return true;
			return false;
		});

	if (iter == m_Collisions.end())
		return false;

	// 이미 맵안에 들어가있었기때문에 Stay인게 확정.
	// 나머지는 갱신할 필요 없음.
	static_cast<CGameObject*>(m_pOwner)->OnCollisionStay(*iter);

	return true;
}

void CCollider::ExitCollision(CCollider* pOtherCollider)
{
	// 충돌을 안했는데 안에 있었다면 Exit상태 처리.
	if (nullptr == pOtherCollider)
		return;

	auto iter = find_if(m_Collisions.begin(), m_Collisions.end(), [&](COLLISIONDESC CollDesc) 
		{
			if (CollDesc.pOtherCollider == pOtherCollider)
				return true;
			return false;
		});

	if (iter == m_Collisions.end())
		return;

	static_cast<CGameObject*>(m_pOwner)->OnCollisionExit(*iter);

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

	for (auto Collision : m_Collisions)
		Collision.pOtherCollider->ExitCollision(this);
	m_Collisions.clear();
}