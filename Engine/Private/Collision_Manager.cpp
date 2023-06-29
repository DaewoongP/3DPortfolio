#include "Collision_Manager.h"
#include "Collider.h"

IMPLEMENT_SINGLETON(CCollision_Manager)

CCollision_Manager::CCollision_Manager()
{
}

HRESULT CCollision_Manager::Add_Collider(COLTYPE eCollisionType, CCollider* pCollider)
{
	if (nullptr == pCollider)
		return E_FAIL;

	m_Colliders[eCollisionType].push_back(pCollider);

	Safe_AddRef(pCollider);

	return S_OK;
}

void CCollision_Manager::Tick()
{
	Collision(COLTYPE_STATIC, COLTYPE_DYNAMIC);
	Collision(COLTYPE_DYNAMIC, COLTYPE_DYNAMIC);

	ClearColliders();
}

void CCollision_Manager::Collision(COLTYPE eSourType, COLTYPE eDestType)
{
	for (auto& pSourCollider : m_Colliders[eSourType])
	{
		for (auto& pDestCollider : m_Colliders[eDestType])
		{
			if (pSourCollider == pDestCollider)
				continue;

			_float3 vCollisionBox;

			if (true == pSourCollider->Intersects(pDestCollider, &vCollisionBox))
			{
				Check_Direction(pSourCollider, pDestCollider, vCollisionBox);
			}
			// 충돌 중이 아니므로 콜라이더에 콜리전이 있을경우 Exit 호출 필요.
			else
			{
				pSourCollider->ExitCollision(pDestCollider);
				pDestCollider->ExitCollision(pSourCollider);
			}
		}
	}
}

void CCollision_Manager::Check_Direction(class CCollider* pSourCollider, class CCollider* pDestCollider, _float3 vCollisionBox)
{
	_float3 vSourCenterPos = pSourCollider->Get_BoundingCenterPosition();
	_float3 vDestCenterPos = pDestCollider->Get_BoundingCenterPosition();

	// x <= y
	// x 최소 (좌우충돌)
	if (vCollisionBox.x <= vCollisionBox.y &&
		vCollisionBox.x < vCollisionBox.z)
	{
		// 포지션이 큰애가 좌충돌.
		// SourCenter X가 더 클경우
		if (vSourCenterPos.x >= vDestCenterPos.x)
		{
			pSourCollider->OnCollision(COLLISIONDESC::COLDIR_LEFT, pDestCollider);
			pDestCollider->OnCollision(COLLISIONDESC::COLDIR_RIGHT, pSourCollider);
		}
		else // DestCenter X가 더 클경우.
		{
			pSourCollider->OnCollision(COLLISIONDESC::COLDIR_RIGHT, pDestCollider);
			pDestCollider->OnCollision(COLLISIONDESC::COLDIR_LEFT, pSourCollider);
		}
	}
	// y <= z
	// y 최소 (상하충돌)
	else if (vCollisionBox.y <= vCollisionBox.z &&
			vCollisionBox.y < vCollisionBox.x)
	{
		// 포지션이 큰애가 하충돌.
		// SourCenter Y가 더 클경우
		if (vSourCenterPos.y >= vDestCenterPos.y)
		{
			pSourCollider->OnCollision(COLLISIONDESC::COLDIR_DOWN, pDestCollider);
			pDestCollider->OnCollision(COLLISIONDESC::COLDIR_UP, pSourCollider);
		}
		else // DestCenter Y가 더 클경우.
		{
			pSourCollider->OnCollision(COLLISIONDESC::COLDIR_UP, pDestCollider);
			pDestCollider->OnCollision(COLLISIONDESC::COLDIR_DOWN, pSourCollider);
		}
	}
	// z <= x
	// z 최소 (앞뒤충돌)
	else if (vCollisionBox.z <= vCollisionBox.x &&
			vCollisionBox.z < vCollisionBox.y)
	{
		// 포지션이 큰애가 뒤충돌.
		// SourCenter Z가 더 클경우
		if (vSourCenterPos.z >= vDestCenterPos.z)
		{
			pSourCollider->OnCollision(COLLISIONDESC::COLDIR_BACK, pDestCollider);
			pDestCollider->OnCollision(COLLISIONDESC::COLDIR_FRONT, pSourCollider);
		}
		else // DestCenter Z가 더 클경우.
		{
			pSourCollider->OnCollision(COLLISIONDESC::COLDIR_FRONT, pDestCollider);
			pDestCollider->OnCollision(COLLISIONDESC::COLDIR_BACK, pSourCollider);
		}
	}
}

void CCollision_Manager::ClearColliders()
{
	for (_uint i = 0; i < COLTYPE_END; ++i)
	{
		for (auto& pCollider : m_Colliders[i])
		{
			Safe_Release(pCollider);
		}
		m_Colliders[i].clear();
	}
}

void CCollision_Manager::Free()
{
	ClearColliders();
}