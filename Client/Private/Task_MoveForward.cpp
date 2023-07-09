#include "..\Public\Task_MoveForward.h"
#include "GameInstance.h"

HRESULT CTask_MoveForward::Initialize(CBlackBoard* pBlackBoard)
{
	if (FAILED(__super::Initialize(pBlackBoard)))
		return E_FAIL;

	m_pTransformCom = static_cast<CTransform*>(m_pBlackBoard->Find_Value(TEXT("Value_Transform")));
	m_isWalk = static_cast<_bool*>(m_pBlackBoard->Find_Value(TEXT("Value_isWalk")));

	return S_OK;
}

CBehavior::STATE CTask_MoveForward::Tick(_double dTimeDelta)
{
	if (nullptr == m_pTransformCom)
	{
		*m_isWalk = false;
		return STATE_FAILED;
	}

	m_pTransformCom->Go_Straight(dTimeDelta);

	m_dRunningAcc += dTimeDelta;

	if (m_dRunningAcc < 3.0)
	{
		*m_isWalk = true;
		return STATE_RUNNING;
	}
	
	m_dRunningAcc = 0.0;

	*m_isWalk = false;

	return STATE_SUCCESS;
}

HRESULT CTask_MoveForward::Reset()
{
	if (FAILED(__super::Reset()))
		return E_FAIL;

	return S_OK;
}

CTask_MoveForward* CTask_MoveForward::Create(CBlackBoard* pBlackBoard)
{
	CTask_MoveForward* pInstance = new CTask_MoveForward();

	if (FAILED(pInstance->Initialize(pBlackBoard)))
	{
		MSG_BOX("Failed to Created CTask_MoveForward");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTask_MoveForward::Free()
{
	__super::Free();
}
