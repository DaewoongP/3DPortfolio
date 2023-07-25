#pragma once
#include "Base.h"

BEGIN(Engine)

class CLayer final : public CBase
{
private:
	explicit CLayer();
	virtual ~CLayer() = default;

public:
	// 현재 레이어의 모든 게임 오브젝트를 반환
	unordered_map<const _tchar*, class CGameObject*>& Get_AllGameObject() { return m_GameObjects; }

public:
	HRESULT			Add_GameObjects(const _tchar* pGameObjectTag, class CGameObject* pGameObject);
	CGameObject*	Find_GameObject(const _tchar* pGameObjectTag);
	HRESULT			Delete_GameObject(const _tchar* pGameObjectTag);
	// 레이어 클리어
	HRESULT			Clear_Layer();
	HRESULT			Clear_DeadObjects();
	HRESULT			SetUp_GameEvent(GAMEEVENT eEvent);

public:
	void Tick(_double dTimeDelta);
	GAMEEVENT Late_Tick(_double dTimeDelta);

	HRESULT ResetStage();

private:
	unordered_map<const _tchar*, class CGameObject*>	m_GameObjects;
	unordered_map<const _tchar*, class CGameObject*>	m_DeadObjects;

public:
	static CLayer* Create();
	virtual void Free() override;
};

END