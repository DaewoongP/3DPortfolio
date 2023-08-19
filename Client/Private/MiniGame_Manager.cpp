#include "..\Public\MiniGame_Manager.h"

IMPLEMENT_SINGLETON(CMiniGame_Manager)

HRESULT CMiniGame_Manager::Initialize()
{
	m_dMaxFireTime = 2.0;
	m_dCoolTime = 0.5;
	Reset_CenterTime(rand() % 1000 / 1000.0 * m_dMaxFireTime);

	return S_OK;
}

void CMiniGame_Manager::Tick(_double dTimeDelta)
{
	if (true == m_isCoolTime)
	{
		m_dCoolTimeAcc += dTimeDelta;
		
		if (m_dCoolTimeAcc > m_dCoolTime)
		{
			m_eState = STATE_WAIT;
			m_dCoolTimeAcc = 0.0;
			m_isCoolTime = false;
		}
		else
			return;
	}
	
	m_dFireTimeAcc += dTimeDelta;

	// 최대시간을 지나서 실패처리.
	if (m_dFireTimeAcc > m_dMaxFireTime)
	{
		m_dFireTimeAcc = 0.0;
		Reset_CenterTime(rand() % 1000 / 1000.0 * m_dMaxFireTime);
		m_eState = STATE_FAIL;
		m_isCoolTime = true;
		return;
	}
	
	if (true == m_isBlocked)
	{
		m_isBlocked = false;

		// 퍼펙트존 사이
		if (m_dFireTimeAcc > m_dCenterTime - m_dPerfectOffsetTime &&
			m_dFireTimeAcc < m_dCenterTime + m_dPerfectOffsetTime)
		{
#ifdef _DEBUG
			cout << "Perfect" << endl;
#endif //_DEBUG
			Reset_CenterTime(rand() % 1000 / 1000.0 * m_dMaxFireTime);
			m_dFireTimeAcc = 0.0;
			m_eState = STATE_PERFECT;
			m_isCoolTime = true;
			return;
		}

		// 그레이트존 사이
		if (m_dFireTimeAcc > m_dCenterTime - m_dGreatOffsetTime &&
			m_dFireTimeAcc < m_dCenterTime + m_dGreatOffsetTime)
		{
#ifdef _DEBUG
			cout << "Great" << endl;
#endif //_DEBUG
			Reset_CenterTime(rand() % 1000 / 1000.0 * m_dMaxFireTime);
			m_dFireTimeAcc = 0.0;
			m_eState = STATE_GREAT;
			m_isCoolTime = true;
			return;
		}
#ifdef _DEBUG
		cout << "Failed" << endl;
#endif //_DEBUG
		Reset_CenterTime(rand() % 1000 / 1000.0 * m_dMaxFireTime);
		m_dFireTimeAcc = 0.0;
		m_eState = STATE_FAIL;
		m_isCoolTime = true;
	}
}

void CMiniGame_Manager::Reset_CenterTime(_double dCenterTime)
{
	m_dCenterTime = dCenterTime;
	m_dPerfectOffsetTime = m_dMaxFireTime * 0.05;
	m_dGreatOffsetTime = m_dPerfectOffsetTime * 3.0;
}

void CMiniGame_Manager::Free()
{
}
