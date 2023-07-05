#include "Collision_Manager.h"
#include "Collider.h"

IMPLEMENT_SINGLETON(CCollision_Manager)

CCollision_Manager::CCollision_Manager()
{
}

HRESULT CCollision_Manager::Add_Collider(COLLISIONDESC::COLTYPE eCollisionType, CCollider* pCollider)
{
	if (nullptr == pCollider)
		return E_FAIL;

	m_Colliders[eCollisionType].push_back(pCollider);

	Safe_AddRef(pCollider);

	return S_OK;
}

void CCollision_Manager::Tick()
{
	Collision(COLLISIONDESC::COLTYPE_PLAYER, COLLISIONDESC::COLTYPE_STATIC);
	Collision(COLLISIONDESC::COLTYPE_PLAYER, COLLISIONDESC::COLTYPE_ENEMY);
	Collision(COLLISIONDESC::COLTYPE_PLAYER, COLLISIONDESC::COLTYPE_ENEMYVISION);
	Collision(COLLISIONDESC::COLTYPE_PLAYER, COLLISIONDESC::COLTYPE_ENEMYWEAPON);
	//Collision(COLTYPE_ENEMY, COLTYPE_STATIC);

	ClearColliders();
}

void CCollision_Manager::Collision(COLLISIONDESC::COLTYPE eSourType, COLLISIONDESC::COLTYPE eDestType)
{
	for (auto& pSourCollider : m_Colliders[eSourType])
	{
		for (auto& pDestCollider : m_Colliders[eDestType])
		{
			if (pSourCollider == pDestCollider)
				continue;

			_float3 vCollisionBox = _float3(0.f, 0.f, 0.f);

			if (true == pSourCollider->Intersects(pDestCollider, &vCollisionBox))
			{
				Check_Direction(eSourType, pSourCollider, eDestType, pDestCollider, vCollisionBox);
			}
		}
	}
}

void CCollision_Manager::Check_Direction(COLLISIONDESC::COLTYPE eSourType, class CCollider* pSourCollider, COLLISIONDESC::COLTYPE eDestType, class CCollider* pDestCollider, _float3 vCollisionBox)
{
	_float3 vSourCenterPos = pSourCollider->Get_BoundingCenterPosition();
	_float3 vDestCenterPos = pDestCollider->Get_BoundingCenterPosition();

	// x <= y
	// x �ּ� (�¿��浹)
	if (vCollisionBox.x <= vCollisionBox.y &&
		vCollisionBox.x < vCollisionBox.z)
	{
		// �������� ū�ְ� ���浹.
		// SourCenter X�� �� Ŭ���
		if (vSourCenterPos.x >= vDestCenterPos.x)
		{
			pSourCollider->OnCollision(eDestType, COLLISIONDESC::COLDIR_LEFT, pDestCollider);
			pDestCollider->OnCollision(eSourType, COLLISIONDESC::COLDIR_RIGHT, pSourCollider);
		}
		else // DestCenter X�� �� Ŭ���.
		{
			pSourCollider->OnCollision(eDestType, COLLISIONDESC::COLDIR_RIGHT, pDestCollider);
			pDestCollider->OnCollision(eSourType, COLLISIONDESC::COLDIR_LEFT, pSourCollider);
		}
	}
	// y <= z
	// y �ּ� (�����浹)
	else if (vCollisionBox.y <= vCollisionBox.z &&
			vCollisionBox.y < vCollisionBox.x)
	{
		// �������� ū�ְ� ���浹.
		// SourCenter Y�� �� Ŭ���
		if (vSourCenterPos.y >= vDestCenterPos.y)
		{
			pSourCollider->OnCollision(eDestType, COLLISIONDESC::COLDIR_DOWN, pDestCollider);
			pDestCollider->OnCollision(eSourType, COLLISIONDESC::COLDIR_UP, pSourCollider);
		}
		else // DestCenter Y�� �� Ŭ���.
		{
			pSourCollider->OnCollision(eDestType, COLLISIONDESC::COLDIR_UP, pDestCollider);
			pDestCollider->OnCollision(eSourType, COLLISIONDESC::COLDIR_DOWN, pSourCollider);
		}
	}
	// z <= x
	// z �ּ� (�յ��浹)
	else if (vCollisionBox.z <= vCollisionBox.x &&
			vCollisionBox.z < vCollisionBox.y)
	{
		// �������� ū�ְ� ���浹.
		// SourCenter Z�� �� Ŭ���
		if (vSourCenterPos.z >= vDestCenterPos.z)
		{
			pSourCollider->OnCollision(eDestType, COLLISIONDESC::COLDIR_BACK, pDestCollider);
			pDestCollider->OnCollision(eSourType, COLLISIONDESC::COLDIR_FRONT, pSourCollider);
		}
		else // DestCenter Z�� �� Ŭ���.
		{
			pSourCollider->OnCollision(eDestType, COLLISIONDESC::COLDIR_FRONT, pDestCollider);
			pDestCollider->OnCollision(eSourType, COLLISIONDESC::COLDIR_BACK, pSourCollider);
		}
	}
	// �ڽ� �浹�� �ƴ�.
	else
	{
		pSourCollider->OnCollision(eDestType, COLLISIONDESC::COLDIR_END, pDestCollider);
		pDestCollider->OnCollision(eSourType, COLLISIONDESC::COLDIR_END, pSourCollider);
	}
}

void CCollision_Manager::ClearColliders()
{
	for (_uint i = 0; i < COLLISIONDESC::COLTYPE_END; ++i)
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