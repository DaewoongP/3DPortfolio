#pragma once
#include "ImWindow.h"
#include "Dummy.h"

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
	vector<class CGameObject*> Get_Objects(CDummy::DUMMYTYPE eType) { 
		return m_Objects[eType];
	}
	void Set_Object(CDummy::DUMMYTYPE eType, CGameObject* pObject);

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;

private:
	CDummy::DUMMYTYPE			m_eCurRadio = { CDummy::DUMMY_NONANIM };
	_int						m_iCurrentListIndex = { 0 };
	_bool						m_bClearButton = { false };
	vector<class CGameObject*>	m_Objects[CDummy::DUMMY_END];
	vector<_char*>				m_ObjectNames[CDummy::DUMMY_END];

private:
	HRESULT Select_Objects();
	HRESULT CurrentObjectListBox();
	HRESULT DeleteObject();
	HRESULT AnimationIndex();

public:
	static CWindow_Object* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END