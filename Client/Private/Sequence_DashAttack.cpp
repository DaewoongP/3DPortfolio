#include "Sequence_DashAttack.h"
#include "Task_LookAt.h"
#include "Task_Dash.h"
#include "Task_Wait.h"
#include "Task_Ready.h"
#include "GameObject.h"

HRESULT CSequence_DashAttack::Initialize(CBlackBoard* pBlackBoard, CDecorator* pDecorator)
{
	if (FAILED(__super::Initialize(pBlackBoard, pDecorator)))
		return E_FAIL;

	if (FAILED(Add_Child(TEXT("Task_LookAt"), CTask_LookAt::Create(pBlackBoard))))
		return E_FAIL;
	if (FAILED(Add_Child(TEXT("Task_Ready"), CTask_Ready::Create(pBlackBoard))))
		return E_FAIL;
	if (FAILED(Add_Child(TEXT("Task_Dash"), CTask_Dash::Create(pBlackBoard))))
		return E_FAIL;
	if (FAILED(Add_Child(TEXT("Task_Wait"), CTask_Wait::Create(pBlackBoard))))
		return E_FAIL;

	return S_OK;
}

CSequence_DashAttack* CSequence_DashAttack::Create(CBlackBoard* pBlackBoard, CDecorator* pDecorator)
{
	CSequence_DashAttack* pInstance = new CSequence_DashAttack();

	if (FAILED(pInstance->Initialize(pBlackBoard, pDecorator)))
	{
		MSG_BOX("Failed to Created CSequence_DashAttack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSequence_DashAttack::Free()
{
	__super::Free();
}