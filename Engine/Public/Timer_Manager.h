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
	// 현재의 TimeDelta 반환
	_double		Get_TimeDelta(const _tchar* pTimerTag);
	void		Set_SlowedTime(const _tchar* pTimerTag, _double dTime);

public:
	// 타이머 태그로 타이머 생성
	HRESULT		Add_Timer(const _tchar* pTimerTag);
	// 타이머를 틱에 맞춰 루프시킴.
	void		Tick_Timer(const _tchar* pTimerTag);
	
private:
	class CTimer*		Find_Timer(const _tchar* pTimerTag);

private:
	unordered_map<const _tchar*, class CTimer*>		m_umapTimers;

public:
	virtual	void Free() override;
};

END
