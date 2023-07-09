#include "..\Public\Selector.h"

CSelector::CSelector()
{
}

HRESULT CSelector::Initialize(CBlackBoard* pBlackBoard, CDecorator* pDecorator)
{
	if (FAILED(__super::Initialize(pBlackBoard, pDecorator)))
		return E_FAIL;

	return S_OK;
}

CBehavior::STATE CSelector::Tick(_double dTimeDelta)
{
	// ���ڷ����� Ȯ��
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

		// ������ ������ ��ȯ�� �ε����� �ƴϸ� ��ŵ.
		if (-1 != m_iPreRunningChildIndex &&
			iIndex != m_iPreRunningChildIndex)
			continue;

		eState = pChild.second->Tick(dTimeDelta);

		if (STATE_SUCCESS == eState)
		{
			m_iPreRunningChildIndex = -1;
			return STATE_SUCCESS;
		}
		else if (STATE_RUNNING == eState)
		{
			m_iPreRunningChildIndex = iIndex;
			return STATE_RUNNING;
		}
		else
			m_iPreRunningChildIndex = -1;
	}

	return STATE_FAILED;
}

void CSelector::Free()
{
	__super::Free();
}
