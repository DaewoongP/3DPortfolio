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
	// 현재 툴에 깔려있는 오브젝트 리스트를 벡터컨테이너로 반환
	vector<class CGameObject*> Get_Objects(CDummy::DUMMYTYPE eType) { 
		return m_Objects[eType];
	}
	// 현재 오브젝트 리스트에 오브젝트를 추가
	void Set_Object(CDummy::DUMMYTYPE eType, CGameObject* pObject);

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;

private:
	// 리스트박스를 맞는 타입으로 활성화 시키기 위한 라디오버튼
	CDummy::DUMMYTYPE			m_eCurRadio = { CDummy::DUMMY_NONANIM };
	// 현재 활성화된 리스트의 인덱스값
	_int						m_iCurrentListIndex = { 0 };
	// 초기화 버튼
	_bool						m_bClearButton = { false };

	// 현재 툴에 깔린 오브젝트들
	vector<class CGameObject*>	m_Objects[CDummy::DUMMY_END];
	// 현재 툴에 깔린 오브젝트들의 이름값들.
	vector<_char*>				m_ObjectNames[CDummy::DUMMY_END];

private:
	HRESULT Select_Objects();
	HRESULT CurrentObjectListBox();
	HRESULT DeleteObject();
	HRESULT ObjectOption();

public:
	static CWindow_Object* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END