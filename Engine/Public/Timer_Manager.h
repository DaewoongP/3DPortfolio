#pragma once

#include "Base.h"

BEGIN(Engine)

class CTimer_Manager : public CBase
{
	DECLARE_SINGLETON(CTimer_Manager)

private:
	explicit CTimer_Manager();
	virtual ~CTimer_Manager() = default;

public:
	// ������ TimeDelta ��ȯ
	_double		Get_TimeDelta(const _tchar* pTimerTag);
	void		Set_SlowedTime(const _tchar* pTimerTag, _double dTime);

public:
	// Ÿ�̸� �±׷� Ÿ�̸� ����
	HRESULT		Add_Timer(const _tchar* pTimerTag);
	// Ÿ�̸Ӹ� ƽ�� ���� ������Ŵ.
	void		Tick_Timer(const _tchar* pTimerTag);
	
private:
	class CTimer*		Find_Timer(const _tchar* pTimerTag);

private:
	unordered_map<const _tchar*, class CTimer*>		m_umapTimers;

public:
	virtual	void Free() override;
};

END
