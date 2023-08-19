#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Client)

class CMiniGame_Manager final : public CBase
{
	DECLARE_SINGLETON(CMiniGame_Manager)

public:
	enum STATE { STATE_PERFECT, STATE_GREAT, STATE_FAIL, STATE_WAIT, STATE_END };

private:
	explicit CMiniGame_Manager() = default;
	virtual ~CMiniGame_Manager() = default;

public:
	HRESULT Add_Enemy(class CEnemy_Sniper* pEnemy_Sniper);
	void Set_Blocked(_bool isBlocked) { m_isBlocked = isBlocked; }
	_double Get_CenterPercent() { return m_dCenterTime / m_dMaxFireTime; }
	_double Get_FirePercent() { return m_dFireTimeAcc / m_dMaxFireTime; }
	_double Get_PerfectOffsetTime() const { return m_dPerfectOffsetTime; }
	_double Get_GreatOffsetTime() const { return m_dGreatOffsetTime; }
	_double Get_MaxTime() const { return m_dMaxFireTime; }
	STATE Get_State() const { return m_eState; }

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
	// 쿨타임
	_double				m_dCoolTime = { 0.0 };
	_double				m_dCoolTimeAcc = { 0.0 };
	_bool				m_isCoolTime = { false };

	// 초기 대기시간
	_double				m_dWaitTime = { 0.0 };
	_double				m_dWaitTimeAcc = { 0.0 };

private:
	STATE				m_eState = { STATE_END };
	_bool				m_isBlocked = { false };

private:
	vector<class CEnemy_Sniper*>	m_Enemy_Snipers;

private:
	void Reset_CenterTime(_double dCenterTime);
	void Select_RandomAttack();
	void Delete_Bullet();

public:
	virtual void Free() override;
};

END