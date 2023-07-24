#include "..\Public\Sequence_Landing.h"
#include "Task_ChaseLook.h"
#include "Task_Landing.h"

HRESULT CSequence_Landing::Initialize(CBlackBoard* pBlackBoard, CDecorator* pDecorator)
{
	if (FAILED(__super::Initialize(pBlackBoard, pDecorator)))
		return E_FAIL;

	if (FAILED(Add_Child(TEXT("Task_ChaseLook"), CTask_ChaseLook::Create(pBlackBoard))))
		return E_FAIL;
	if (FAILED(Add_Child(TEXT("Task_Landing"), CTask_Landing::Create(pBlackBoard))))
		return E_FAIL;

	return S_OK;
}

CSequence_Landing* CSequence_Landing::Create(CBlackBoard* pBlackBoard, CDecorator* pDecorator)
{
	CSequence_Landing* pInstance = new CSequence_Landing();

	if (FAILED(pInstance->Initialize(pBlackBoard, pDecorator)))
	{
		MSG_BOX("Failed to Created CSequence_Landing");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSequence_Landing::Free()
{
	__super::Free();
}