#include "..\Public\Task_MoveRight.h"
#include "GameInstance.h"

HRESULT CTask_MoveRight::Initialize(CBlackBoard* pBlackBoard)
{
	if (FAILED(__super::Initialize(pBlackBoard)))
		return E_FAIL;

	m_pTransformCom = static_cast<CTransform*>(m_pBlackBoard->Find_Value(TEXT("Value_Transform")));
	m_dMoveTime = static_cast<_double*>(m_pBlackBoard->Find_Value(TEXT("Value_MoveTime")));
	m_isMoveRight = static_cast<_bool*>(m_pBlackBoard->Find_Value(TEXT("Value_isMoveRight")));

	return S_OK;
}

CBehavior::STATE CTask_MoveRight::Tick(_double dTimeDelta)
{
	if (nullptr == m_pTransformCom ||
		nullptr == m_isMoveRight)
		return STATE_FAILED;

	m_dRunningAcc += dTimeDelta;

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (*m_dMoveTime >= m_dRunningAcc)
	{
		m_pTransformCom->Go_Right(dTimeDelta);

		*m_isMoveRight = true;

		return STATE_RUNNING;
	}

	*m_isMoveRight = false;
	m_dRunningAcc = 0.0;

	return STATE_SUCCESS;
}

HRESULT CTask_MoveRight::Reset()
{
	if (FAILED(__super::Reset()))
		return E_FAIL;

	return S_OK;
}

CTask_MoveRight* CTask_MoveRight::Create(CBlackBoard* pBlackBoard)
{
	CTask_MoveRight* pInstance = new CTask_MoveRight();

	if (FAILED(pInstance->Initialize(pBlackBoard)))
	{
		MSG_BOX("Failed to Created CTask_MoveRight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTask_MoveRight::Free()
{
	__super::Free();
}
