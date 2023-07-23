#include "..\Public\RandomChoose.h"
#include "Calculator.h"

CRandomChoose::CRandomChoose()
{
}

HRESULT CRandomChoose::Initialize(CBlackBoard* pBlackBoard, CDecorator* pDecorator)
{
	if (FAILED(__super::Initialize(pBlackBoard, pDecorator)))
		return E_FAIL;

	return S_OK;
}

CBehavior::STATE CRandomChoose::Tick(_double dTimeDelta)
{
	// 데코레이터 확인
	for (auto& pDecorator : m_Decorators)
	{
		if (false == pDecorator->Check_Decorator(m_pBlackBoard))
			return STATE_FAILED;
	}

	CBehavior::STATE eState;

	_int iIndex = { -1 };

	// Running 처리
	if (-1 != m_iPreRunningChildIndex)
	{
		iIndex = m_iPreRunningChildIndex;
	}
	else
	{
		CCalculator* pCalculator = CCalculator::GetInstance();
		Safe_AddRef(pCalculator);

		iIndex = pCalculator->RandomChoose(m_ChildWeights, (_uint)m_Childs.size());

		Safe_Release(pCalculator);

		if (-1 == iIndex)
			return STATE_FAILED;
	}

	eState = m_Childs[iIndex].second->Tick(dTimeDelta);

	if (STATE_RUNNING == eState)
	{
		m_iPreRunningChildIndex = iIndex;
	}
	else
		m_iPreRunningChildIndex = -1;

	return eState;
}

void CRandomChoose::Free()
{
	__super::Free();
}
