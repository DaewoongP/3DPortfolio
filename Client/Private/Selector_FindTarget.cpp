#include "..\Public\Selector_FindTarget.h"
#include "Sequence_Patrol.h"

HRESULT CSelector_FindTarget::Initialize(CBlackBoard* pBlackBoard, CDecorator* pDecorator)
{
	if (FAILED(__super::Initialize(pBlackBoard, pDecorator)))
		return E_FAIL;

	if (FAILED(Add_Child(TEXT("Sequence_Patrol"), CSequence_Patrol::Create(pBlackBoard))))
		return E_FAIL;

	return S_OK;
}

CSelector_FindTarget* CSelector_FindTarget::Create(CBlackBoard* pBlackBoard, CDecorator* pDecorator)
{
	CSelector_FindTarget* pInstance = new CSelector_FindTarget();

	if (FAILED(pInstance->Initialize(pBlackBoard, pDecorator)))
	{
		MSG_BOX("Failed to Created CSelector_FindTarget");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSelector_FindTarget::Free()
{
	__super::Free();
}
