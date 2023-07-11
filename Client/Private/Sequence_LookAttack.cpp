#include "Sequence_LookAttack.h"
#include "Task_LookAt.h"
#include "Task_Attack.h"
#include "Task_FailedWait.h"

HRESULT CSequence_LookAttack::Initialize(CBlackBoard* pBlackBoard, CDecorator* pDecorator)
{
	if (FAILED(__super::Initialize(pBlackBoard, pDecorator)))
		return E_FAIL;

	if (FAILED(Add_Child(TEXT("Task_LookAt"), CTask_LookAt::Create(pBlackBoard))))
		return E_FAIL;
	if (FAILED(Add_Child(TEXT("Task_Attack"), CTask_Attack::Create(pBlackBoard))))
		return E_FAIL;

	return S_OK;
}

CSequence_LookAttack* CSequence_LookAttack::Create(CBlackBoard* pBlackBoard, CDecorator* pDecorator)
{
	CSequence_LookAttack* pInstance = new CSequence_LookAttack();

	if (FAILED(pInstance->Initialize(pBlackBoard, pDecorator)))
	{
		MSG_BOX("Failed to Created CSequence_LookAttack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSequence_LookAttack::Free()
{
	__super::Free();
}
