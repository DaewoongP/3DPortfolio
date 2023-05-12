#pragma once

#include "Base.h"

BEGIN(Engine)

class CObject_Manager final : public CBase
{
	DECLARE_SINGLETON(CObject_Manager)
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	void	Set_CurLevel(const _uint iCurLevel) { m_iCurLevel = iCurLevel; }
public:
	HRESULT Reserve_Containers(_uint iNumLevels);
	HRESULT Add_Prototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObject(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pLayerTag, void* pArg);
	void	Clear_LevelResources(_uint iLevelIndex);
	
public:
	void	Tick(_double dTimeDelta);
	void	Late_Tick(_double dTimeDelta);
private:
	typedef unordered_map<const _tchar*, class CLayer*>	LAYERS;
	unordered_map<const _tchar*, class CGameObject*>	m_Prototypes;
	LAYERS*												m_pLayers = { nullptr };
	_uint												m_iNumLevels = { 0 };
	_uint												m_iCurLevel = { 0 };
private:
	class CGameObject*	Find_Prototype(const _tchar* pPrototypeTag);
	class CLayer*		Find_Layer(_uint iLevelIndex, const _tchar* pLayerTag);

public:
	virtual void Free() override;
};

END