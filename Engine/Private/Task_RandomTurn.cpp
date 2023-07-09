#include "..\Public\Task_RandomTurn.h"
#include "Transform.h"

HRESULT CTask_RandomTurn::Initialize(CBlackBoard* pBlackBoard)
{
	if (FAILED(__super::Initialize(pBlackBoard)))
		return E_FAIL;

	m_pOwnerTransformCom = static_cast<CTransform*>(m_pBlackBoard->Find_Value(TEXT("Value_Transform")));

	return S_OK;
}

CBehavior::STATE CTask_RandomTurn::Tick(_double dTimeDelta)
{
	m_pOwnerTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), dTimeDelta);
	
	m_dRunningAcc += dTimeDelta;

	if (m_dRunningAcc < 1.0)
		return STATE_RUNNING;

	m_dRunningAcc = 0.0;

	return STATE_SUCCESS;
}

CTask_RandomTurn* CTask_RandomTurn::Create(CBlackBoard* pBlackBoard)
{
	CTask_RandomTurn* pInstance = new CTask_RandomTurn();

	if (FAILED(pInstance->Initialize(pBlackBoard)))
	{
		MSG_BOX("Failed to Created CTask_RandomTurn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTask_RandomTurn::Free()
{
	__super::Free();
}
