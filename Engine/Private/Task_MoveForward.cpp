#include "..\Public\Task_MoveForward.h"
#include "Transform.h"

HRESULT CTask_MoveForward::Initialize(CBlackBoard* pBlackBoard)
{
	if (FAILED(__super::Initialize(pBlackBoard)))
		return E_FAIL;

	m_pOwnerTransformCom = static_cast<CTransform*>(m_pBlackBoard->Find_Value(TEXT("Value_Transform")));

	return S_OK;
}

CBehavior::STATE CTask_MoveForward::Tick(_double dTimeDelta)
{
	m_pOwnerTransformCom->Go_Straight(dTimeDelta);

	m_dRunningAcc += dTimeDelta;

	if (m_dRunningAcc < 3.0)
		return STATE_RUNNING;

	m_dRunningAcc = 0.0;

	return STATE_SUCCESS;
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
