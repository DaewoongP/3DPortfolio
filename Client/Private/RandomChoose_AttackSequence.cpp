#include "..\Public\RandomChoose_AttackSequence.h"
#include "Task_AttackSequence1.h"
#include "Task_AttackSequence2.h"

HRESULT CRandomChoose_AttackSequence::Initialize(CBlackBoard* pBlackBoard, CDecorator* pDecorator)
{
	if (FAILED(__super::Initialize(pBlackBoard, pDecorator)))
		return E_FAIL;

	m_ChildWeights.push_back(0.5f);
	if (FAILED(Add_Child(TEXT("Task_AttackSequence1"), CTask_AttackSequence1::Create(pBlackBoard))))
		return E_FAIL;
	m_ChildWeights.push_back(0.5f);
	if (FAILED(Add_Child(TEXT("Task_AttackSequence2"), CTask_AttackSequence2::Create(pBlackBoard))))
		return E_FAIL;

	return S_OK;
}

CRandomChoose_AttackSequence* CRandomChoose_AttackSequence::Create(CBlackBoard* pBlackBoard, CDecorator* pDecorator)
{
	CRandomChoose_AttackSequence* pInstance = new CRandomChoose_AttackSequence();

	if (FAILED(pInstance->Initialize(pBlackBoard, pDecorator)))
	{
		MSG_BOX("Failed to Created CRandomChoose_AttackSequence");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRandomChoose_AttackSequence::Free()
{
	__super::Free();
}