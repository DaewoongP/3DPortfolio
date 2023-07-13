#include "..\Public\Selector_Hammer.h"
#include "Sequence_Patrol.h"
#include "Sequence_LookAttack.h"

HRESULT CSelector_Hammer::Initialize(CBlackBoard* pBlackBoard, CDecorator* pDecorator)
{
	if (FAILED(__super::Initialize(pBlackBoard, pDecorator)))
		return E_FAIL;

	if (FAILED(Add_Child(TEXT("Sequence_Patrol"), CSequence_Patrol::Create(pBlackBoard))))
		return E_FAIL;
	if (FAILED(Add_Child(TEXT("Sequence_LookAttack"), CSequence_LookAttack::Create(pBlackBoard))))
		return E_FAIL;

	m_Decorators.push_back(
		// Patrol 데코레이터
		CDecorator::Create([&](CBlackBoard* pDecoBlackBoard)->_bool
			{
				// 스탑이 불리면 false
				void* pDead = pDecoBlackBoard->Find_Value(TEXT("Value_isDead"));
				if (true == *reinterpret_cast<_bool*>(pDead))
					return false;

				return true;
			})
	);

	return S_OK;
}

CSelector_Hammer* CSelector_Hammer::Create(CBlackBoard* pBlackBoard, CDecorator* pDecorator)
{
	CSelector_Hammer* pInstance = new CSelector_Hammer();

	if (FAILED(pInstance->Initialize(pBlackBoard, pDecorator)))
	{
		MSG_BOX("Failed to Created CSelector_Hammer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSelector_Hammer::Free()
{
	__super::Free();
}
