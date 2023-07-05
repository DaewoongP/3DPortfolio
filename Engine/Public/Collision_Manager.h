#pragma once
#include "Base.h"

BEGIN(Engine)

class CCollision_Manager final : public CBase
{
	DECLARE_SINGLETON(CCollision_Manager)

private:
	explicit CCollision_Manager();
	virtual ~CCollision_Manager() = default;

public:
	HRESULT Add_Collider(COLLISIONDESC::COLTYPE eCollisionType, class CCollider* pCollider);
	void Tick();

public:
	void Collision(COLLISIONDESC::COLTYPE eSourType, COLLISIONDESC::COLTYPE eDestType);
	void Check_Direction(COLLISIONDESC::COLTYPE eSourType, class CCollider* pSourCollider, COLLISIONDESC::COLTYPE eDestType, class CCollider* pDestCollider, _float3 vCollisionBox);

	void ClearColliders();

private:
	// �ݶ��̴��� Ư�� ���̾�? ���� �����ϱ� ���� ���������̳�.
	vector<class CCollider*>	m_Colliders[COLLISIONDESC::COLTYPE_END];

public:
	virtual void Free() override;
};

END