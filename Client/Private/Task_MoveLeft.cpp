#include "..\Public\Task_MoveLeft.h"
#include "GameInstance.h"

HRESULT CTask_MoveLeft::Initialize(CBlackBoard* pBlackBoard)
{
	if (FAILED(__super::Initialize(pBlackBoard)))
		return E_FAIL;

	m_pTransformCom = static_cast<CTransform*>(m_pBlackBoard->Find_Value(TEXT("Value_Transform")));
	m_dMoveTime = static_cast<_double*>(m_pBlackBoard->Find_Value(TEXT("Value_MoveTime")));
	m_isMoveLeft = static_cast<_bool*>(m_pBlackBoard->Find_Value(TEXT("Value_isMoveLeft")));

	return S_OK;
}

CBehavior::STATE CTask_MoveLeft::Tick(_double dTimeDelta)
{
	if (nullptr == m_pTransformCom ||
		nullptr == m_isMoveLeft)
		return STATE_FAILED;

	m_dRunningAcc += dTimeDelta;

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (*m_dMoveTime >= m_dRunningAcc)
	{
		m_pTransformCom->Go_Left(dTimeDelta);

		*m_isMoveLeft = true;

		return STATE_RUNNING;
	}

	*m_isMoveLeft = false;
	m_dRunningAcc = 0.0;

	return STATE_SUCCESS;
}

HRESULT CTask_MoveLeft::Reset()
{
	if (FAILED(__super::Reset()))
		return E_FAIL;

	return S_OK;
}

CTask_MoveLeft* CTask_MoveLeft::Create(CBlackBoard* pBlackBoard)
{
	CTask_MoveLeft* pInstance = new CTask_MoveLeft();

	if (FAILED(pInstance->Initialize(pBlackBoard)))
	{
		MSG_BOX("Failed to Created CTask_MoveLeft");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTask_MoveLeft::Free()
{
	__super::Free();
}
