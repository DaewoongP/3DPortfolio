#include "..\Public\Sequence_Flying.h"
#include "Task_Fly.h"
#include "Task_ChaseLook.h"
#include "RandomChoose_Move.h"
#include "RandomChoose_FlyingAttack.h"

HRESULT CSequence_Flying::Initialize(CBlackBoard* pBlackBoard, CDecorator* pDecorator)
{
	if (FAILED(__super::Initialize(pBlackBoard, pDecorator)))
		return E_FAIL;

	if (FAILED(Add_Child(TEXT("Task_Fly"), CTask_Fly::Create(pBlackBoard))))
		return E_FAIL;
	if (FAILED(Add_Child(TEXT("Task_ChaseLook"), CTask_ChaseLook::Create(pBlackBoard))))
		return E_FAIL;
	if (FAILED(Add_Child(TEXT("RandomChoose_Move"), CRandomChoose_Move::Create(pBlackBoard))))
		return E_FAIL;
	if (FAILED(Add_Child(TEXT("RandomChoose_FlyingAttack"), CRandomChoose_FlyingAttack::Create(pBlackBoard))))
		return E_FAIL;

	m_Decorators.push_back(
		CDecorator::Create([&](CBlackBoard* pDecoBlackBoard)->_bool
			{
				// 패턴 카운트가 일정이상 넘어가면 false, 어택 시컨스 실행.
				void* pCurPatternCnt = pDecoBlackBoard->Find_Value(TEXT("Value_CurPatternCnt"));
				void* pMaxPatternCnt = pDecoBlackBoard->Find_Value(TEXT("Value_MaxPatternCnt"));
				if (*reinterpret_cast<_uint*>(pMaxPatternCnt) <= *reinterpret_cast<_uint*>(pCurPatternCnt))
					return false;

				return true;
			})
	);

	return S_OK;
}

CSequence_Flying* CSequence_Flying::Create(CBlackBoard* pBlackBoard, CDecorator* pDecorator)
{
	CSequence_Flying* pInstance = new CSequence_Flying();

	if (FAILED(pInstance->Initialize(pBlackBoard, pDecorator)))
	{
		MSG_BOX("Failed to Created CSequence_Flying");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSequence_Flying::Free()
{
	__super::Free();
}