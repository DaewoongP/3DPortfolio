#pragma once

#include "Base.h"

BEGIN(Engine9)

class CObject_Manager final : public CBase
{
	DECLARE_SINGLETON(CObject_Manager)
private:
	explicit CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	HRESULT Add_Prototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObject(const _tchar* pPrototypeTag, const _tchar* pLayerTag, void* pArg);
	
public:
	void	Tick(_double dTimeDelta);
	void	Late_Tick(_double dTimeDelta);

private:
	unordered_map<const _tchar*, class CGameObject*>	m_Prototypes;
	unordered_map<const _tchar*, class CGameObject*>	m_Layers;
	
private:
	class CGameObject*	Find_Prototype(const _tchar* pPrototypeTag);

public:
	virtual void Free() override;
};

END