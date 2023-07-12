#include "Task_Ready.h"

HRESULT CTask_Ready::Initialize(CBlackBoard* pBlackBoard)
{
	if (FAILED(__super::Initialize(pBlackBoard)))
		return E_FAIL;

	m_isReady = static_cast<_bool*>(m_pBlackBoard->Find_Value(TEXT("Value_isReady")));
	m_dReadyTime = static_cast<_double*>(m_pBlackBoard->Find_Value(TEXT("Value_ReadyTime")));

	return S_OK;
}

CBehavior::STATE CTask_Ready::Tick(_double dTimeDelta)
{
	if (nullptr == m_dReadyTime ||
		nullptr == m_isReady)
		return STATE_FAILED;

	m_dRunningAcc += dTimeDelta;

	if (m_dRunningAcc < *m_dReadyTime)
	{
		*m_isReady = true;
		return STATE_RUNNING;
	}

	*m_isReady = false;
	m_dRunningAcc = 0.0;

	return STATE_SUCCESS;
}

HRESULT CTask_Ready::Reset()
{
	return S_OK;
}

CTask_Ready* CTask_Ready::Create(CBlackBoard* pBlackBoard)
{
	CTask_Ready* pInstance = new CTask_Ready();

	if (FAILED(pInstance->Initialize(pBlackBoard)))
	{
		MSG_BOX("Failed to Created CTask_Ready");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTask_Ready::Free()
{
	__super::Free();
}