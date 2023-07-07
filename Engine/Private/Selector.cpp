#include "..\Public\Selector.h"

CSelector::CSelector()
{
}

CBehavior::STATE CSelector::Tick(class CBlackBoard* pBlackBoard, _double dTimeDelta)
{
	CBehavior::STATE eState;

	for (auto& pChild : m_Childs)
	{
		eState = pChild->Tick(pBlackBoard, dTimeDelta);

		if (STATE_SUCCESS == eState)
			return STATE_SUCCESS;
		else if (STATE_RUNNING == eState)
			return STATE_RUNNING;
	}

	return STATE_FAILED;
}

void CSelector::Free()
{
	__super::Free();
}
