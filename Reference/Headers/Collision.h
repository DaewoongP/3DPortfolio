#pragma once
#include "Base.h"

BEGIN(Engine)

class CCollision final : public CBase
{
public:
	enum COLTYPE { COL_STATIC, COL_DYNAMIC, COL_END };
	enum COLDIR { COLDIR_LEFT, COLDIR_RIGHT, COLDIR_UP, COLDIR_DOWN, COLDIR_FRONT, COLDIR_BACK, COLDIR_END };
	enum COLSTATE { COLSTATE_ENTER, COLSTATE_STAY, COLSTATE_EXIT, COLSTATE_END };

public:
	typedef struct tagCollisionDesc
	{
		_bool			isPreCol = { false };
		_bool			isCurCol = { false };
		COLDIR			ColDir = { COLDIR_END };
		COLSTATE		ColState = { COLSTATE_END };
	}COLLISIONDESC;

private:
	explicit CCollision();
	virtual ~CCollision() = default;

public:
	HRESULT Add_Collider(COLTYPE eCollisionType, class CCollider* pCollider);
	void Tick();
	void ClearColliders();

private:
	// 콜라이더를 특정 레이어? 별로 관리하기 위한 벡터컨테이너.
	vector<class CCollider*>	m_Colliders[COL_END];

public:
	virtual void Free() override;
};

END