#pragma once
#include "Base.h"

BEGIN(Engine)

class CLayer final : public CBase
{
private:
	explicit CLayer();
	virtual ~CLayer() = default;

public:
	// ���� ���̾��� ��� ���� ������Ʈ�� ��ȯ
	unordered_map<const _tchar*, class CGameObject*>& Get_AllGameObject() { return m_GameObjects; }

public:
	HRESULT			Add_GameObjects(const _tchar* pGameObjectTag, class CGameObject* pGameObject);
	CGameObject*	Find_GameObject(const _tchar* pGameObjectTag);
	HRESULT			Delete_GameObject(const _tchar* pGameObjectTag);
	// ���̾� Ŭ����
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