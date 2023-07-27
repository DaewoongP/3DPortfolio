#include "..\Public\Sequence_Patrol.h"
#include "Task_MoveForward.h"
#include "Task_RandomTurn.h"
#include "Task_RandomWait.h"
#include "GameObject.h"

HRESULT CSequence_Patrol::Initialize(CBlackBoard* pBlackBoard, CDecorator* pDecorator)
{
	if (FAILED(__super::Initialize(pBlackBoard, pDecorator)))
		return E_FAIL;

	if (FAILED(Add_Child(TEXT("Task_RandomTurn"), CTask_RandomTurn::Create(pBlackBoard))))
		return E_FAIL;
	if (FAILED(Add_Child(TEXT("Task_MoveForward"), CTask_MoveForward::Create(pBlackBoard))))
		return E_FAIL;
	if (FAILED(Add_Child(TEXT("Task_RandomWait"), CTask_RandomWait::Create(pBlackBoard))))
		return E_FAIL;

	m_Decorators.push_back(
		// Patrol 데코레이터
		CDecorator::Create([&](CBlackBoard* pDecoBlackBoard)->_bool
			{
				// 스탑이 불리면 false
				void* pDead = pDecoBlackBoard->Find_Value(TEXT("Value_isDead"));
				if (true == *reinterpret_cast<_bool*>(pDead))
					return false;

				// 타겟을 찾으면 false
				void* pTarget = pDecoBlackBoard->Find_Value(TEXT("Value_Target"));
				CGameObject* pObject = *reinterpret_cast<CGameObject**>(pTarget);
				if (nullptr != pObject)
				{
					// 타겟이 죽어있으면 true
					if (GAME_OBJECT_DEAD == pObject->Get_GameEvent())
						return true;
					return false;
				}
				
				return true;
			})
	);

	return S_OK;
}

CSequence_Patrol* CSequence_Patrol::Create(CBlackBoard* pBlackBoard, CDecorator* pDecorator)
{
	CSequence_Patrol* pInstance = new CSequence_Patrol();

	if (FAILED(pInstance->Initialize(pBlackBoard, pDecorator)))
	{
		MSG_BOX("Failed to Created CSequence_Patrol");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSequence_Patrol::Free()
{
	__super::Free();
}
