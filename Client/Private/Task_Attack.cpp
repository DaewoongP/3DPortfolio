#include "..\Public\Task_Attack.h"

HRESULT CTask_Attack::Initialize(CBlackBoard* pBlackBoard)
{
	if (FAILED(__super::Initialize(pBlackBoard)))
		return E_FAIL;

	m_dAttackCooltime = static_cast<_double*>(m_pBlackBoard->Find_Value(TEXT("Value_AttackCoolTime")));

	return S_OK;
}

CBehavior::STATE CTask_Attack::Tick(_double dTimeDelta)
{
	if (nullptr == m_dAttackCooltime)
		return STATE_FAILED;
	
	m_dRunningAcc += dTimeDelta;

	if (m_dRunningAcc < *m_dAttackCooltime)
	{
		return STATE_RUNNING;
	}

	m_dRunningAcc = 0.0;

	return STATE_SUCCESS;
}

HRESULT CTask_Attack::Reset()
{
	return S_OK;
}

CTask_Attack* CTask_Attack::Create(CBlackBoard* pBlackBoard)
{
	CTask_Attack* pInstance = new CTask_Attack();

	if (FAILED(pInstance->Initialize(pBlackBoard)))
	{
		MSG_BOX("Failed to Created CTask_Attack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTask_Attack::Free()
{
	__super::Free();
}
