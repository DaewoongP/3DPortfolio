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
	CBehavior::STATE eState;

	for (auto& pChild : m_Childs)
	{
		eState = pChild->Tick(dTimeDelta);

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
