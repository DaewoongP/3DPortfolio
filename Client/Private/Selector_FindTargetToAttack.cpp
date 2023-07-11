#include "..\Public\Selector_FindTargetToAttack.h"
#include "Sequence_Patrol.h"
#include "Sequence_LookAttack.h"
#include "Task_Attack.h"

HRESULT CSelector_FindTargetToAttack::Initialize(CBlackBoard* pBlackBoard, CDecorator* pDecorator)
{
	if (FAILED(__super::Initialize(pBlackBoard, pDecorator)))
		return E_FAIL;

	if (FAILED(Add_Child(TEXT("Sequence_Patrol"), CSequence_Patrol::Create(pBlackBoard))))
		return E_FAIL;
	if (FAILED(Add_Child(TEXT("Sequence_LookAttack"), CSequence_LookAttack::Create(pBlackBoard))))
		return E_FAIL;

	m_Decorators.push_back(
		// Patrol ���ڷ�����
		CDecorator::Create([&](CBlackBoard* pDecoBlackBoard)->_bool
			{
				// ��ž�� �Ҹ��� false
				void* pDead = pDecoBlackBoard->Find_Value(TEXT("Value_isDead"));
				if (true == *reinterpret_cast<_bool*>(pDead))
					return false;

				return true;
			})
	);

	return S_OK;
}

CSelector_FindTargetToAttack* CSelector_FindTargetToAttack::Create(CBlackBoard* pBlackBoard, CDecorator* pDecorator)
{
	CSelector_FindTargetToAttack* pInstance = new CSelector_FindTargetToAttack();

	if (FAILED(pInstance->Initialize(pBlackBoard, pDecorator)))
	{
		MSG_BOX("Failed to Created CSelector_FindTargetToAttack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSelector_FindTargetToAttack::Free()
{
	__super::Free();
}
