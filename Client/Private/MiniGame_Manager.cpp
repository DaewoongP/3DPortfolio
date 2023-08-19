#include "..\Public\MiniGame_Manager.h"

IMPLEMENT_SINGLETON(CMiniGame_Manager)

HRESULT CMiniGame_Manager::Initialize()
{
	m_dMaxFireTime = 10.0;

	m_dCenterTime = 4.0;
	m_dPerfectOffsetTime = 1.0;
	m_dGreatOffsetTime = m_dPerfectOffsetTime + 1.0;

	return S_OK;
}

void CMiniGame_Manager::Tick(_double dTimeDelta)
{
	m_dFireTimeAcc += dTimeDelta;

	// �ִ�ð��� ������ ����ó��.
	if (m_dFireTimeAcc > m_dMaxFireTime)
	{
		m_dFireTimeAcc = 0.0;
		m_eState = STATE_FAIL;
		return;
	}
	
	if (true == m_isBlocked)
	{
		m_isBlocked = false;

		// ����Ʈ�� ����
		if (m_dFireTimeAcc > m_dCenterTime - m_dPerfectOffsetTime &&
			m_dFireTimeAcc < m_dCenterTime + m_dPerfectOffsetTime)
		{
			m_dFireTimeAcc = 0.0;
			m_eState = STATE_PERFECT;
			return;
		}

		// �׷���Ʈ�� ����
		if (m_dFireTimeAcc > m_dCenterTime - m_dGreatOffsetTime &&
			m_dFireTimeAcc < m_dCenterTime + m_dGreatOffsetTime)
		{
			m_dFireTimeAcc = 0.0;
			m_eState = STATE_GREAT;
			return;
		}

		m_dFireTimeAcc = 0.0;
		m_eState = STATE_FAIL;
	}
}

void CMiniGame_Manager::Free()
{
}
