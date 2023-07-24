#include "..\Public\Sequence_Charge.h"
#include "Task_LookAt.h"
#include "Task_Charge.h"
#include "Task_Landing.h"

HRESULT CSequence_Charge::Initialize(CBlackBoard* pBlackBoard, CDecorator* pDecorator)
{
	if (FAILED(__super::Initialize(pBlackBoard, pDecorator)))
		return E_FAIL;

	if (FAILED(Add_Child(TEXT("Task_LookAt"), CTask_LookAt::Create(pBlackBoard))))
		return E_FAIL;
	if (FAILED(Add_Child(TEXT("Task_Charge"), CTask_Charge::Create(pBlackBoard))))
		return E_FAIL;
	if (FAILED(Add_Child(TEXT("Task_LookAt2"), CTask_LookAt::Create(pBlackBoard))))
		return E_FAIL;

	return S_OK;
}

CSequence_Charge* CSequence_Charge::Create(CBlackBoard* pBlackBoard, CDecorator* pDecorator)
{
	CSequence_Charge* pInstance = new CSequence_Charge();

	if (FAILED(pInstance->Initialize(pBlackBoard, pDecorator)))
	{
		MSG_BOX("Failed to Created CSequence_Charge");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSequence_Charge::Free()
{
	__super::Free();
}