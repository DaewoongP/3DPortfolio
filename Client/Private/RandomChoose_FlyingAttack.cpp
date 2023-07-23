#include "..\Public\RandomChoose_FlyingAttack.h"
#include "Task_ThrowBomb.h"
#include "Task_Summon.h"

HRESULT CRandomChoose_FlyingAttack::Initialize(CBlackBoard* pBlackBoard, CDecorator* pDecorator)
{
	if (FAILED(__super::Initialize(pBlackBoard, pDecorator)))
		return E_FAIL;

	m_ChildWeights.push_back(0.5f);
	if (FAILED(Add_Child(TEXT("Task_ThrowBomb"), CTask_ThrowBomb::Create(pBlackBoard))))
		return E_FAIL;
	m_ChildWeights.push_back(0.5f);
	if (FAILED(Add_Child(TEXT("Task_Summon"), CTask_Summon::Create(pBlackBoard))))
		return E_FAIL;

	return S_OK;
}

CRandomChoose_FlyingAttack* CRandomChoose_FlyingAttack::Create(CBlackBoard* pBlackBoard, CDecorator* pDecorator)
{
	CRandomChoose_FlyingAttack* pInstance = new CRandomChoose_FlyingAttack();

	if (FAILED(pInstance->Initialize(pBlackBoard, pDecorator)))
	{
		MSG_BOX("Failed to Created CRandomChoose_FlyingAttack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRandomChoose_FlyingAttack::Free()
{
	__super::Free();
}