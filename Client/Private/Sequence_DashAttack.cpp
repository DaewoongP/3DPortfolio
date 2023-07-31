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