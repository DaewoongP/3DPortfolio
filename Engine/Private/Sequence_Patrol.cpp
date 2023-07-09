#include "..\Public\Sequence_Patrol.h"
#include "Task_MoveForward.h"
#include "Task_RandomTurn.h"

HRESULT CSequence_Patrol::Initialize(CBlackBoard* pBlackBoard, CDecorator* pDecorator)
{
	if (FAILED(__super::Initialize(pBlackBoard, pDecorator)))
		return E_FAIL;

	if (FAILED(Add_Child(TEXT("Task_MoveForward"), CTask_MoveForward::Create(pBlackBoard))))
		return E_FAIL;
	if (FAILED(Add_Child(TEXT("Task_RandomTurn"), CTask_RandomTurn::Create(pBlackBoard))))
		return E_FAIL;

	return S_OK;
}

CSequence_Patrol* CSequence_Patrol::Create(CBlackBoard* pBlackBoard, CDecorator* pDecorator)
{
	CSequence_Patrol* pInstance = new CSequence_Patrol();

	if (FAILED(pInstance->Initialize(pBlackBoard, pDecorator)))
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
