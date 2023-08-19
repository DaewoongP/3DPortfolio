#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Client)

class CMiniGame_Manager final : public CBase
{
	DECLARE_SINGLETON(CMiniGame_Manager)

public:
	enum STATE { STATE_PERFECT, STATE_GREAT, STATE_FAIL, STATE_END };

private:
	explicit CMiniGame_Manager() = default;
	virtual ~CMiniGame_Manager() = default;

public:
	void Set_Blocked(_bool isBlocked) { m_isBlocked = isBlocked; }
	_double Get_CenterPercent() { return m_dCenterTime / m_dMaxFireTime; }

public:
	HRESULT Initialize();
	void Tick(_double dTimeDelta);

private:
	_double				m_dFireTimeAcc = { 0.0 };
	// 타이밍의 중앙값이 되는 값
	_double				m_dCenterTime = { 0.0 };
	// 중앙값으로부터 퍼펙트존
	_double				m_dPerfectOffsetTime = { 0.0 };
	// 중앙값으로부터 그레이트존
	_double				m_dGreatOffsetTime = { 0.0 };
	// 최대 시간
	_double				m_dMaxFireTime = { 0.0 };

private:
	STATE				m_eState = { STATE_END };
	_bool				m_isBlocked = { false };

public:
	virtual void Free() override;
};

END