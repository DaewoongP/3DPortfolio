#pragma once
#include "Base.h"

BEGIN(Engine)

class CLayer final : public CBase
{
private:
	explicit CLayer();
	virtual ~CLayer() = default;

public:
	HRESULT Add_GameObjects(const _tchar* pGameObjectTag, class CGameObject* pGameObject);
	CGameObject* Find_GameObject(const _tchar* pGameObjectTag);
	unordered_map<const _tchar*, class CGameObject*>& Get_AllGameObject() { return m_GameObjects; }

public:
	void Tick(_double dTimeDelta);
	void Late_Tick(_double dTimeDelta);

private:
	unordered_map<const _tchar*, class CGameObject*>	m_GameObjects;


public:
	static CLayer* Create();
	virtual void Free() override;
};

END