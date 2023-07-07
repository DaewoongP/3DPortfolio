#include "..\Public\Sequence.h"

CSequence::CSequence()
{
}

CBehavior::STATE CSequence::Tick(class CBlackBoard* pBlackBoard, _double dTimeDelta)
{
	CBehavior::STATE eState;

	for (auto& pChild : m_Childs)
	{
		eState = pChild->Tick(pBlackBoard, dTimeDelta);

		if (STATE_FAILED == eState)
			return STATE_FAILED;
		else if (STATE_RUNNING == eState)
			return STATE_RUNNING;
	}

	return STATE_SUCCESS;
}

void CSequence::Free()
{
	__super::Free();
}
