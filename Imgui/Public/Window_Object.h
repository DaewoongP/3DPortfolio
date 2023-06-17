#pragma once
#include "ImWindow.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Tool)

class CWindow_Object : public CImWindow
{
private:
	explicit CWindow_Object();
	virtual ~CWindow_Object() = default;

public:
	vector<class CGameObject*> Get_Objects() { return m_Objects; }
	void Set_Object(CGameObject* pObject);

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;

private:
	_int						m_iCurrentListIndex = { 0 };
	_int						m_iAnimationIndex = { 0 };
	_double						m_dAnimationSpeed = { 1.0 };
	_bool						m_bClearButton = { false };
	vector<class CGameObject*>	m_Objects;
	vector<_char*>				m_ObjectNames;

private:
	HRESULT CurrentObjectListBox();
	HRESULT DeleteObject();
	HRESULT AnimationIndex();
	HRESULT AnimationSpeed(class CAnimModel* pAnimModel);

public:
	static CWindow_Object* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END