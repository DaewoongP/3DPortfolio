#include "..\Public\RandomChoose_Move.h"
#include "Task_MoveFront.h"
#include "Task_MoveBack.h"
#include "Task_MoveRight.h"
#include "Task_MoveLeft.h"

HRESULT CRandomChoose_Move::Initialize(CBlackBoard* pBlackBoard, CDecorator* pDecorator)
{
	if (FAILED(__super::Initialize(pBlackBoard, pDecorator)))
		return E_FAIL;

	m_ChildWeights.push_back(0.2f);
	if (FAILED(Add_Child(TEXT("Task_MoveFront"), CTask_MoveFront::Create(pBlackBoard))))
		return E_FAIL;
	m_ChildWeights.push_back(0.2f);
	if (FAILED(Add_Child(TEXT("Task_MoveBack"), CTask_MoveBack::Create(pBlackBoard))))
		return E_FAIL;
	m_ChildWeights.push_back(0.3f);
	if (FAILED(Add_Child(TEXT("Task_MoveRight"), CTask_MoveRight::Create(pBlackBoard))))
		return E_FAIL;
	m_ChildWeights.push_back(0.3f);
	if (FAILED(Add_Child(TEXT("Task_MoveLeft"), CTask_MoveLeft::Create(pBlackBoard))))
		return E_FAIL;

	return S_OK;
}

CRandomChoose_Move* CRandomChoose_Move::Create(CBlackBoard* pBlackBoard, CDecorator* pDecorator)
{
	CRandomChoose_Move* pInstance = new CRandomChoose_Move();

	if (FAILED(pInstance->Initialize(pBlackBoard, pDecorator)))
	{
		MSG_BOX("Failed to Created CRandomChoose_Move");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRandomChoose_Move::Free()
{
	__super::Free();
}