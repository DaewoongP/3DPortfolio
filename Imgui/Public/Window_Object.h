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
	// ���� ���� ����ִ� ������Ʈ ����Ʈ�� ���������̳ʷ� ��ȯ
	vector<class CGameObject*> Get_Objects(CDummy::DUMMYTYPE eType) { 
		return m_Objects[eType];
	}
	// ���� ������Ʈ ����Ʈ�� ������Ʈ�� �߰�
	void Set_Object(CDummy::DUMMYTYPE eType, CGameObject* pObject);

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;

private:
	// ����Ʈ�ڽ��� �´� Ÿ������ Ȱ��ȭ ��Ű�� ���� ������ư
	CDummy::DUMMYTYPE			m_eCurRadio = { CDummy::DUMMY_NONANIM };
	// ���� Ȱ��ȭ�� ����Ʈ�� �ε�����
	_int						m_iCurrentListIndex = { 0 };
	// �ʱ�ȭ ��ư
	_bool						m_bClearButton = { false };

	// ���� ���� �� ������Ʈ��
	vector<class CGameObject*>	m_Objects[CDummy::DUMMY_END];
	// ���� ���� �� ������Ʈ���� �̸�����.
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