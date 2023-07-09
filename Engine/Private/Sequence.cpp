#include "..\Public\Sequence.h"

CSequence::CSequence()
{
}

HRESULT CSequence::Initialize(CBlackBoard* pBlackBoard)
{
	if (FAILED(__super::Initialize(pBlackBoard)))
		return E_FAIL;

	return S_OK;
}

CBehavior::STATE CSequence::Tick(_double dTimeDelta)
{
	// 데코레이터 확인
	for (auto& pDecorator : m_Decorators)
	{
		if (false == pDecorator->Check_Decorator(m_pBlackBoard))
			return STATE_FAILED;
	}

	CBehavior::STATE eState;

	_int iIndex = { -1 };

	for (auto& pChild : m_Childs)
	{
		++iIndex;

		// 이전에 러닝을 반환한 인덱스가 아니면 스킵.
		if (-1 != m_iPreRunningChildIndex && 
			iIndex != m_iPreRunningChildIndex)
			continue;

		eState = pChild.second->Tick(dTimeDelta);

		if (STATE_FAILED == eState)
		{
			m_iPreRunningChildIndex = -1;
			return STATE_FAILED;
		}
		else if (STATE_RUNNING == eState)
		{
			m_iPreRunningChildIndex = iIndex;
			return STATE_RUNNING;
		}
	}

	m_iPreRunningChildIndex = -1;
	return STATE_SUCCESS;
}

void CSequence::Free()
{
	__super::Free();
}
