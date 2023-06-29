#pragma once
#include "Base.h"

BEGIN(Engine)

class CCollision_Manager final : public CBase
{
	DECLARE_SINGLETON(CCollision_Manager)

public:
	enum COLTYPE { COLTYPE_STATIC, COLTYPE_DYNAMIC, COLTYPE_END };

private:
	explicit CCollision_Manager();
	virtual ~CCollision_Manager() = default;

public:
	HRESULT Add_Collider(COLTYPE eCollisionType, class CCollider* pCollider);
	void Tick();

public:
	void Collision(COLTYPE eSourType, COLTYPE eDestType);
	void Check_Direction(class CCollider* pSourCollider, class CCollider* pDestCollider, _float3 vCollisionBox);

	void ClearColliders();

private:
	// �ݶ��̴��� Ư�� ���̾�? ���� �����ϱ� ���� ���������̳�.
	vector<class CCollider*>	m_Colliders[COLTYPE_END];

public:
	virtual void Free() override;
};

END