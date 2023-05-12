#pragma once
#include "Base.h"

BEGIN(Engine)

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	HRESULT Add_GameObjects(class CGameObject* pGameObject);
	void Tick(_double TimeDelta);
	void Late_Tick(_double TimeDelta);

private:
	list<class CGameObject*>				m_GameObjects;

public:
	static CLayer* Create();
	virtual void Free() override;
};

END