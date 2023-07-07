#include "..\Public\Selector.h"

CSelector::CSelector()
{
}

HRESULT CSelector::Initialize(CBlackBoard* pBlackBoard)
{
	if (FAILED(__super::Initialize(pBlackBoard)))
		return E_FAIL;

	return S_OK;
}

CBehavior::STATE CSelector::Tick(_double dTimeDelta)
{
	CBehavior::STATE eState;

	for (auto& pChild : m_Childs)
	{
		eState = pChild->Tick(dTimeDelta);

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
