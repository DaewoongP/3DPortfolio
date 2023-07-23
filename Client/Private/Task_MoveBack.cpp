#include "..\Public\Task_MoveBack.h"
#include "GameInstance.h"

HRESULT CTask_MoveBack::Initialize(CBlackBoard* pBlackBoard)
{
	if (FAILED(__super::Initialize(pBlackBoard)))
		return E_FAIL;

	m_pTransformCom = static_cast<CTransform*>(m_pBlackBoard->Find_Value(TEXT("Value_Transform")));
	m_dMoveTime = static_cast<_double*>(m_pBlackBoard->Find_Value(TEXT("Value_MoveTime")));
	m_isMoveBack = static_cast<_bool*>(m_pBlackBoard->Find_Value(TEXT("Value_isMoveBack")));

	return S_OK;
}

CBehavior::STATE CTask_MoveBack::Tick(_double dTimeDelta)
{
	if (nullptr == m_pTransformCom ||
		nullptr == m_isMoveBack)
		return STATE_FAILED;

	m_dRunningAcc += dTimeDelta;

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (*m_dMoveTime >= m_dRunningAcc)
	{
		m_pTransformCom->Go_Backward(dTimeDelta);

		*m_isMoveBack = true;

		return STATE_RUNNING;
	}

	*m_isMoveBack = false;
	m_dRunningAcc = 0.0;

	return STATE_SUCCESS;
}

HRESULT CTask_MoveBack::Reset()
{
	if (FAILED(__super::Reset()))
		return E_FAIL;

	return S_OK;
}

CTask_MoveBack* CTask_MoveBack::Create(CBlackBoard* pBlackBoard)
{
	CTask_MoveBack* pInstance = new CTask_MoveBack();

	if (FAILED(pInstance->Initialize(pBlackBoard)))
	{
		MSG_BOX("Failed to Created CTask_MoveBack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTask_MoveBack::Free()
{
	__super::Free();
}
