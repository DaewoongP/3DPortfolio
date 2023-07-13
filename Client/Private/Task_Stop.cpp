#include "..\Public\Task_Stop.h"

HRESULT CTask_Stop::Initialize(CBlackBoard* pBlackBoard)
{
	if (FAILED(__super::Initialize(pBlackBoard)))
		return E_FAIL;

	m_isStop = static_cast<_bool*>(m_pBlackBoard->Find_Value(TEXT("Value_isStop")));

	return S_OK;
}

CBehavior::STATE CTask_Stop::Tick(_double dTimeDelta)
{
	if (nullptr == m_isStop)
		return STATE_FAILED;

	// 스탑이 true이면 Running을 걸어 잠궈버림.
	if (true == *m_isStop)
		return STATE_RUNNING;

	return STATE_SUCCESS;
}

HRESULT CTask_Stop::Reset()
{
	*m_isStop = false;

	if (FAILED(__super::Reset()))
		return E_FAIL;

	return S_OK;
}

CTask_Stop* CTask_Stop::Create(CBlackBoard* pBlackBoard)
{
	CTask_Stop* pInstance = new CTask_Stop();

	if (FAILED(pInstance->Initialize(pBlackBoard)))
	{
		MSG_BOX("Failed to Created CTask_Stop");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTask_Stop::Free()
{
	__super::Free();
}
