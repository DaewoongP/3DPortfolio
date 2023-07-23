#include "..\Public\Task_MoveFront.h"
#include "GameInstance.h"

HRESULT CTask_MoveFront::Initialize(CBlackBoard* pBlackBoard)
{
	if (FAILED(__super::Initialize(pBlackBoard)))
		return E_FAIL;

	m_pTransformCom = static_cast<CTransform*>(m_pBlackBoard->Find_Value(TEXT("Value_Transform")));
	m_dMoveTime = static_cast<_double*>(m_pBlackBoard->Find_Value(TEXT("Value_MoveTime")));
	m_isMoveFront = static_cast<_bool*>(m_pBlackBoard->Find_Value(TEXT("Value_isMoveFront")));

	return S_OK;
}

CBehavior::STATE CTask_MoveFront::Tick(_double dTimeDelta)
{
	if (nullptr == m_pTransformCom ||
		nullptr == m_isMoveFront)
		return STATE_FAILED;

	m_dRunningAcc += dTimeDelta;

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (*m_dMoveTime >= m_dRunningAcc)
	{
		m_pTransformCom->Go_Straight(dTimeDelta);

		*m_isMoveFront = true;

		return STATE_RUNNING;
	}
	
	*m_isMoveFront = false;
	m_dRunningAcc = 0.0;

	return STATE_SUCCESS;
}

HRESULT CTask_MoveFront::Reset()
{
	if (FAILED(__super::Reset()))
		return E_FAIL;

	return S_OK;
}

CTask_MoveFront* CTask_MoveFront::Create(CBlackBoard* pBlackBoard)
{
	CTask_MoveFront* pInstance = new CTask_MoveFront();

	if (FAILED(pInstance->Initialize(pBlackBoard)))
	{
		MSG_BOX("Failed to Created CTask_MoveFront");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTask_MoveFront::Free()
{
	__super::Free();
}
