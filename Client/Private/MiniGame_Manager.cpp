#include "..\Public\MiniGame_Manager.h"
#include "Enemy_Sniper.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CMiniGame_Manager)

HRESULT CMiniGame_Manager::Add_Enemy(CEnemy_Sniper* pEnemy_Sniper)
{
	if (nullptr == pEnemy_Sniper)
		return E_FAIL;

	m_Enemy_Snipers.push_back(pEnemy_Sniper);
	Safe_AddRef(pEnemy_Sniper);

	//Select_RandomAttack();

	return S_OK;
}

HRESULT CMiniGame_Manager::Initialize()
{
	m_dMaxFireTime = 1.5;
	m_dCoolTime = 0.7;
	Reset_CenterTime(rand() % 1000 / 1000.0 * m_dMaxFireTime);

	m_dWaitTime = 3.0;
	m_eState = STATE_WAIT;

	m_iMaxScore = 20;

	m_isFirst = true;

	ZEROMEM(&m_vScore);

	return S_OK;
}

void CMiniGame_Manager::Tick(_double dTimeDelta)
{
	if (m_dWaitTimeAcc < m_dWaitTime)
	{
		m_dWaitTimeAcc += dTimeDelta;
		return;
	}

	if (true == m_isFirst)
	{
		Reset_CenterTime(rand() % 1000 / 1000.0 * m_dMaxFireTime);
		Select_RandomAttack();
		m_dFireTimeAcc = 0.0;
		m_isCoolTime = true;

		m_isFirst = false;
	}

	Check_Score();

	if (true == m_isGameFinished)
	{
		m_eState = STATE_WAIT;
		m_dCoolTimeAcc = 0.0;
		return;
	}
	
	if (true == m_isCoolTime)
	{
		m_dCoolTimeAcc += dTimeDelta;
		
		if (m_dCoolTimeAcc > m_dCoolTime)
		{
			m_eState = STATE_WAIT;
			m_dCoolTimeAcc = 0.0;
			m_isCoolTime = false;
		}
		return;
	}
	
	m_dFireTimeAcc += dTimeDelta;

	// 최대시간을 지나서 실패처리.
	if (m_dFireTimeAcc > m_dCenterTime + m_dGreatOffsetTime * 0.5)
	{
		m_dFireTimeAcc = 0.0;
		Reset_CenterTime(rand() % 1000 / 1000.0 * m_dMaxFireTime);
		Select_RandomAttack();
		m_eState = STATE_FAIL;
		m_isCoolTime = true;
		++m_vScore.z;
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
			m_eState = STATE_PERFECT;
			m_iScore += 3;
			++m_vScore.x;
			Delete_Bullet();
		}
		// 그레이트존 사이
		else if (m_dFireTimeAcc > m_dCenterTime - m_dGreatOffsetTime &&
			m_dFireTimeAcc < m_dCenterTime + m_dGreatOffsetTime)
		{
#ifdef _DEBUG
			cout << "Great" << endl;
#endif //_DEBUG
			m_eState = STATE_GREAT;
			m_iScore += 1;
			++m_vScore.y;
			Delete_Bullet();
		}
		else
		{
#ifdef _DEBUG
			cout << "Failed" << endl;
#endif //_DEBUG
			m_eState = STATE_FAIL;
			++m_vScore.z;
		}

		Reset_CenterTime(rand() % 1000 / 1000.0 * m_dMaxFireTime);
		Select_RandomAttack();
		m_dFireTimeAcc = 0.0;
		m_isCoolTime = true;
	}
}

void CMiniGame_Manager::Reset_CenterTime(_double dCenterTime)
{
	// 안전장치
	if (dCenterTime < 0.5)
		dCenterTime += 0.5;

	m_dCenterTime = dCenterTime;
	m_dPerfectOffsetTime = m_dMaxFireTime * 0.05;
	m_dGreatOffsetTime = m_dPerfectOffsetTime * 3.0;
}

void CMiniGame_Manager::Select_RandomAttack()
{
	if (0 == m_Enemy_Snipers.size())
		return;

	_uint iRand = rand() % m_Enemy_Snipers.size();
	m_Enemy_Snipers[iRand]->Attack(m_dCoolTime + m_dCenterTime + m_dGreatOffsetTime);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	pGameInstance->Play_Sound(TEXT("Pistol_Shot%d.ogg"), 4, CSound_Manager::SOUND_PISTOL, 0.3f, true);
	Safe_Release(pGameInstance);
}

void CMiniGame_Manager::Delete_Bullet()
{
	for (auto& pEnemy_Sniper : m_Enemy_Snipers)
		pEnemy_Sniper->Delete_Bullet();
}

void CMiniGame_Manager::Check_Score()
{
	if (m_iScore < m_iMaxScore)
		return;

	Delete_Bullet();

	for (auto& pEnemy_Sniper : m_Enemy_Snipers)
	{
		pEnemy_Sniper->Set_GameEvent(GAME_OBJECT_DEAD);
		Safe_Release(pEnemy_Sniper);
	}

	m_Enemy_Snipers.clear();

	m_iScore = m_iMaxScore;

	m_isGameFinished = true;
}

void CMiniGame_Manager::Free()
{
	for (auto& pEnemy_Sniper : m_Enemy_Snipers)
		Safe_Release(pEnemy_Sniper);
	m_Enemy_Snipers.clear();
}
