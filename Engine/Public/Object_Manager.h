#pragma once
#include "Base.h"

BEGIN(Engine)

class CObject_Manager final : public CBase
{
	DECLARE_SINGLETON(CObject_Manager)

private:
	explicit CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	class CGameObject*	Get_LastGameObject();
	HRESULT				Set_LastGameObject(class CGameObject* pGameObject);

public:
	HRESULT Reserve_Containers(_uint iNumLevels);
	HRESULT Add_Prototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObject(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pLayerTag, const _tchar* pObjectTag, void* pArg);
	class CGameObject* Clone_GameObject(const _tchar* pPrototypeTag, void* pArg);
	void	Clear_LevelResources(_uint iLevelIndex);
	
	class CGameObject*	Find_Prototype(const _tchar* pPrototypeTag);
	class CGameObject*	Find_GameObject(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pGameObjectTag);
	class CLayer*		Find_Layer(_uint iLevelIndex, const _tchar* pLayerTag);
	class CLayer*		Make_Layer(_uint iLevelIndex, const _tchar* pLayerTag);
	HRESULT				Delete_GameObject(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pGameObjectTag);
	// 레벨에 맞는 레이어안의 게임오브젝트들을 모두 삭제.
	HRESULT				Clear_Layer(_uint iLevelIndex, const _tchar* pLayerTag);
	HRESULT				Clear_DeadObjects(_uint iLevelIndex, const _tchar* pLayerTag);
	HRESULT				SetUp_GameEvent(_uint iLevelIndex, const _tchar* pLayerTag, GAMEEVENT eEvent);

public:
	void	Tick(_double dTimeDelta);
	void	Late_Tick(_double dTimeDelta);
	void	Reset_Stage();

private:
	typedef unordered_map<const _tchar*, class CLayer*>		LAYERS;
	unordered_map<const _tchar*, class CGameObject*>	m_Prototypes;	

private:
	LAYERS*				m_pLayers = { nullptr };
	_uint				m_iNumLevels = { 0 };

	class CGameObject*	m_pLastGameObject = { nullptr };

public:
	virtual void Free() override;
};

END