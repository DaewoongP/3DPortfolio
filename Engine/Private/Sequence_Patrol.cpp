#include "..\Public\Sequence_Patrol.h"
#include "Task_MoveForward.h"

HRESULT CSequence_Patrol::Initialize(CBlackBoard* pBlackBoard)
{
	if (FAILED(__super::Initialize(pBlackBoard)))
		return E_FAIL;

	if (FAILED(Add_Child(TEXT("Task_MoveForward"), CTask_MoveForward::Create(pBlackBoard))))
		return E_FAIL;

	return S_OK;
}

CSequence_Patrol* CSequence_Patrol::Create(CBlackBoard* pBlackBoard)
{
	CSequence_Patrol* pInstance = new CSequence_Patrol();

	if (FAILED(pInstance->Initialize(pBlackBoard)))
	{
		MSG_BOX("Failed to Created CSequence_Patrol");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSequence_Patrol::Free()
{
	__super::Free();
}
