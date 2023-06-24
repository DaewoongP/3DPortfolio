#pragma once
#include "ImWindow.h"
#include "Dummy.h"

BEGIN(Tool)

class CWindow_ObjectOptions : public CImWindow
{
private:
	explicit CWindow_ObjectOptions();
	virtual ~CWindow_ObjectOptions() = default;

public:
	// �ɼ��� �ٲ� ��ü ����
	void Set_CurrentDummy(CDummy::DUMMYTYPE eDummyType, class CDummy* pDummy);

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;

private:
	class CDummy*				m_pCurrentDummy = { nullptr };
	class CModel*				m_pCurrentModel = { nullptr };
	CDummy::DUMMYTYPE			m_eCurrentDummyType = { CDummy::DUMMY_END };

private: /* For.Animations */
	_char						m_szAnimationName[MAX_STR] = "";
	// ���� ���� ��ü �ִϸ��̼� ����
	_uint						m_iNumAnimations = { 0 };
	// �ִϸ��̼� �� ���� ��� ���� �ִϸ��̼��� �ε���
	_int						m_iAnimationIndex = { 0 };
	// ���� ��� ���� �ִϸ��̼��� ������ ����
	_uint						m_iAnimationFrames = { 0 };
	// ���� ��� ���� �������� �ε���
	_uint						m_iCurrentAnimationFrame = { 0 };
	_bool						m_bPauseButton = { false };

private:
	class CAnimationNotify* m_pAnimationNotify = { nullptr };

private: /* For. Animations */
	HRESULT AnimationIndex();
	HRESULT AnimationPause();
	HRESULT AnimationSpeed(_double dTimeDelta);

	// �ִϸ��̼��� Ư�� �����ӿ��� ó���Ǵ� ����
	HRESULT AnimationNotify(_double dTimeDelta);

private: /* For. Collider */
	HRESULT AddCollider();

private: /* For. Collider */
	_float3						m_ColliderExtents[3];

public:
	static CWindow_ObjectOptions* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END