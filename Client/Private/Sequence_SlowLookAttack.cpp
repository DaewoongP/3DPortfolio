#include "..\Public\Sequence_SlowLookAttack.h"
#include "Task_Ready.h"
#include "Task_ChaseLook.h"
#include "Task_Attack.h"
#include "Task_Wait.h"

HRESULT CSequence_SlowLookAttack::Initialize(CBlackBoard* pBlackBoard, CDecorator* pDecorator)
{
	if (FAILED(__super::Initialize(pBlackBoard, pDecorator)))
		return E_FAIL;

	if (FAILED(Add_Child(TEXT("Task_ChaseLook"), CTask_ChaseLook::Create(pBlackBoard))))
		return E_FAIL;
	if (FAILED(Add_Child(TEXT("Task_Ready"), CTask_Ready::Create(pBlackBoard))))
		return E_FAIL;
	if (FAILED(Add_Child(TEXT("Task_Attack"), CTask_Attack::Create(pBlackBoard))))
		return E_FAIL;
	if (FAILED(Add_Child(TEXT("Task_Wait"), CTask_Wait::Create(pBlackBoard))))
		return E_FAIL;

	return S_OK;
}

CSequence_SlowLookAttack* CSequence_SlowLookAttack::Create(CBlackBoard* pBlackBoard, CDecorator* pDecorator)
{
	CSequence_SlowLookAttack* pInstance = new CSequence_SlowLookAttack();

	if (FAILED(pInstance->Initialize(pBlackBoard, pDecorator)))
	{
		MSG_BOX("Failed to Created CSequence_SlowLookAttack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSequence_SlowLookAttack::Free()
{
	__super::Free();
}
