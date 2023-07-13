#include "Task_Wait.h"

HRESULT CTask_Wait::Initialize(CBlackBoard* pBlackBoard)
{
	if (FAILED(__super::Initialize(pBlackBoard)))
		return E_FAIL;

	m_isWait = static_cast<_bool*>(m_pBlackBoard->Find_Value(TEXT("Value_isWait")));
	m_dWaitTime = static_cast<_double*>(m_pBlackBoard->Find_Value(TEXT("Value_WaitTime")));

	return S_OK;
}

CBehavior::STATE CTask_Wait::Tick(_double dTimeDelta)
{
	if (nullptr == m_dWaitTime ||
		nullptr == m_isWait)
		return STATE_FAILED;

	m_dRunningAcc += dTimeDelta;

	if (m_dRunningAcc < *m_dWaitTime)
	{
		*m_isWait = true;
		return STATE_RUNNING;
	}

	*m_isWait = false;
	m_dRunningAcc = 0.0;

	return STATE_SUCCESS;
}

HRESULT CTask_Wait::Reset()
{
	*m_isWait = false;

	if (FAILED(__super::Reset()))
		return E_FAIL;

	return S_OK;
}

CTask_Wait* CTask_Wait::Create(CBlackBoard* pBlackBoard)
{
	CTask_Wait* pInstance = new CTask_Wait();

	if (FAILED(pInstance->Initialize(pBlackBoard)))
	{
		MSG_BOX("Failed to Created CTask_Wait");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTask_Wait::Free()
{
	__super::Free();
}