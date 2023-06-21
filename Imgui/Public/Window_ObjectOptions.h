#pragma once
#include "ImWindow.h"
#include "Dummy.h"

BEGIN(Tool)

class CWindow_ObjectOptions : public CImWindow
{
public:
	struct Funcs // graph style
	{
		static float Sin(void*, int i) { return sinf(i * 0.1f); }
		static float Saw(void*, int i) { return (i & 1) ? 1.0f : -1.0f; }
		static _float FrameSpeedSaw(void* pArg, _int i) 
		{
			vector<_float> FrameSpeed;
			FrameSpeed = *reinterpret_cast<vector<_float>*>(pArg);
			if (0 == FrameSpeed.size())
				return 0;
			return FrameSpeed[i];
		}
	};

private:
	explicit CWindow_ObjectOptions();
	virtual ~CWindow_ObjectOptions() = default;

public:
	// ���� �ִϸ��̼��� ������ ���ǵ尡 ����ִ� ���������̳� ��ȯ
	vector<_float> Get_FrameSpeeds() const { return m_FrameSpeeds; }
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
	// ���� ��� ���� �ִϸ��̼��� ������ �ӵ��� ���� ���� �����̳�
	vector<_float>				m_FrameSpeeds;

	// ������׷��� �׸��� ���� �Լ������� ����
	_float						(*m_FrameSpeedFunc)(void*, _int);

private: /* For. Animations */
	HRESULT AnimationIndex();
	HRESULT AnimationPause();
	HRESULT AnimationSpeed();

private: /* For. Collider */
	HRESULT AddCollider();

public:
	static CWindow_ObjectOptions* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END