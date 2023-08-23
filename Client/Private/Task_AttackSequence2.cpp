#include "..\Public\Task_AttackSequence2.h"
#include "GameInstance.h"
#include "GameObject.h"

HRESULT CTask_AttackSequence2::Initialize(CBlackBoard* pBlackBoard)
{
	if (FAILED(__super::Initialize(pBlackBoard)))
		return E_FAIL;

	m_dAttackSequence2Time = static_cast<_double*>(m_pBlackBoard->Find_Value(TEXT("Value_AttackSequence2Time")));
	m_isAttackSequence2 = static_cast<_bool*>(m_pBlackBoard->Find_Value(TEXT("Value_isAttackSequence2")));
	m_pTransformCom = static_cast<CTransform*>(m_pBlackBoard->Find_Value(TEXT("Value_Transform")));

	return S_OK;
}

CBehavior::STATE CTask_AttackSequence2::Tick(_double dTimeDelta)
{
	if (nullptr == m_dAttackSequence2Time ||
		nullptr == m_isAttackSequence2)
		return STATE_FAILED;

	m_dRunningAcc += dTimeDelta;

	if (*m_dAttackSequence2Time < m_dRunningAcc)
	{
		*m_isAttackSequence2 = false;
		m_dRunningAcc = 0.0;

		*static_cast<_bool*>(m_pBlackBoard->Find_Value(TEXT("Value_isAttackFinished"))) = true;

		return STATE_SUCCESS;
	}

	*m_isAttackSequence2 = true;

	CGameObject* pTarget = *reinterpret_cast<CGameObject**>(m_pBlackBoard->Find_Value(TEXT("Value_Target")));
	if (nullptr == pTarget)
		return STATE_FAILED;

	m_pTransformCom->LookAt(pTarget->Get_Transform()->Get_State(CTransform::STATE_POSITION), true);

	return STATE_RUNNING;
}

HRESULT CTask_AttackSequence2::Reset()
{
	if (FAILED(__super::Reset()))
		return E_FAIL;

	*m_isAttackSequence2 = false;
	m_dRunningAcc = 0.0;

	return S_OK;
}

CTask_AttackSequence2* CTask_AttackSequence2::Create(CBlackBoard* pBlackBoard)
{
	CTask_AttackSequence2* pInstance = new CTask_AttackSequence2();

	if (FAILED(pInstance->Initialize(pBlackBoard)))
	{
		MSG_BOX("Failed to Created CTask_AttackSequence2");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTask_AttackSequence2::Free()
{
	__super::Free();
}

