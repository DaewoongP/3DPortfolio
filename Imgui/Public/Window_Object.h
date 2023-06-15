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
	void Set_Object(CGameObject* pObject);

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;

private:
	_int						m_iCurrentListIndex = { 0 };
	vector<class CGameObject*>	m_Objects;
	vector<_char*>				m_ObjectNames;

private:
	HRESULT CurrentObjectListBox();

public:
	static CWindow_Object* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END