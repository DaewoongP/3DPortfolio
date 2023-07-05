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
	// 콜라이더를 특정 레이어? 별로 관리하기 위한 벡터컨테이너.
	vector<class CCollider*>	m_Colliders[COLTYPE_END];

public:
	virtual void Free() override;
};

END