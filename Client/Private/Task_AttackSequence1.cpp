#include "..\Public\Task_AttackSequence1.h"

HRESULT CTask_AttackSequence1::Initialize(CBlackBoard* pBlackBoard)
{
	if (FAILED(__super::Initialize(pBlackBoard)))
		return E_FAIL;

	m_dAttackSequence1Time = static_cast<_double*>(m_pBlackBoard->Find_Value(TEXT("Value_AttackSequence1Time")));
	m_isAttackSequence1 = static_cast<_bool*>(m_pBlackBoard->Find_Value(TEXT("Value_isAttackSequence1")));

	return S_OK;
}

CBehavior::STATE CTask_AttackSequence1::Tick(_double dTimeDelta)
{
	if (nullptr == m_dAttackSequence1Time ||
		nullptr == m_isAttackSequence1)
		return STATE_FAILED;

	m_dRunningAcc += dTimeDelta;

	if (*m_dAttackSequence1Time < m_dRunningAcc)
	{
		*m_isAttackSequence1 = false;
		m_dRunningAcc = 0.0;

		*static_cast<_bool*>(m_pBlackBoard->Find_Value(TEXT("Value_isAttackFinished"))) = true;

		return STATE_SUCCESS;
	}
	
	*m_isAttackSequence1 = true;
	

	return STATE_RUNNING;
}

HRESULT CTask_AttackSequence1::Reset()
{
	if (FAILED(__super::Reset()))
		return E_FAIL;

	return S_OK;
}

CTask_AttackSequence1* CTask_AttackSequence1::Create(CBlackBoard* pBlackBoard)
{
	CTask_AttackSequence1* pInstance = new CTask_AttackSequence1();

	if (FAILED(pInstance->Initialize(pBlackBoard)))
	{
		MSG_BOX("Failed to Created CTask_AttackSequence1");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTask_AttackSequence1::Free()
{
	__super::Free();
}

