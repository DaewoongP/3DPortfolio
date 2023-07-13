#include "..\Public\Task_RandomWait.h"

HRESULT CTask_RandomWait::Initialize(CBlackBoard* pBlackBoard)
{
	if (FAILED(__super::Initialize(pBlackBoard)))
		return E_FAIL;

	m_iMaxWaitTime = static_cast<_uint*>(m_pBlackBoard->Find_Value(TEXT("Value_MaxWaitTime")));
	m_isWait = static_cast<_bool*>(m_pBlackBoard->Find_Value(TEXT("Value_isWait")));

	return S_OK;
}

CBehavior::STATE CTask_RandomWait::Tick(_double dTimeDelta)
{
	if (nullptr == m_iMaxWaitTime ||
		nullptr == m_isWait)
		return STATE_FAILED;

	m_dRunningAcc += dTimeDelta;

	if (m_dRunningAcc < _double(rand() % (*m_iMaxWaitTime + 1)))
	{
		*m_isWait = true;
		return STATE_RUNNING;
	}

	*m_isWait = false;
	m_dRunningAcc = 0.0;

	return STATE_SUCCESS;
}

HRESULT CTask_RandomWait::Reset()
{
	*m_isWait = false;

	if (FAILED(__super::Reset()))
		return E_FAIL;

	return S_OK;
}

CTask_RandomWait* CTask_RandomWait::Create(CBlackBoard* pBlackBoard)
{
	CTask_RandomWait* pInstance = new CTask_RandomWait();

	if (FAILED(pInstance->Initialize(pBlackBoard)))
	{
		MSG_BOX("Failed to Created CTask_RandomWait");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTask_RandomWait::Free()
{
	__super::Free();
}