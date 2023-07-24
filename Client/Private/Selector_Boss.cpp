#include "..\Public\Selector_Boss.h"
#include "Sequence_Flying.h"
#include "Selector_BossPattern.h"
#include "GameObject.h"

HRESULT CSelector_Boss::Initialize(CBlackBoard* pBlackBoard, CDecorator* pDecorator)
{
	if (FAILED(__super::Initialize(pBlackBoard, pDecorator)))
		return E_FAIL;

	if (FAILED(Add_Child(TEXT("Sequence_Flying"), CSequence_Flying::Create(pBlackBoard))))
		return E_FAIL;
	if (FAILED(Add_Child(TEXT("Selector_BossPattern"), CSelector_BossPattern::Create(pBlackBoard))))
		return E_FAIL;

	m_Decorators.push_back(
		// Patrol 데코레이터
		CDecorator::Create([&](CBlackBoard* pDecoBlackBoard)->_bool
			{
				// 스탑이 불리면 false
				void* pDead = pDecoBlackBoard->Find_Value(TEXT("Value_isDead"));
				if (true == *reinterpret_cast<_bool*>(pDead))
					return false;

				// 타겟이 없으면 false
				void* pTarget = pDecoBlackBoard->Find_Value(TEXT("Value_Target"));
				if (nullptr == *reinterpret_cast<CGameObject**>(pTarget))
					return false;

				return true;
			})
	);

	return S_OK;
}

CSelector_Boss* CSelector_Boss::Create(CBlackBoard* pBlackBoard, CDecorator* pDecorator)
{
	CSelector_Boss* pInstance = new CSelector_Boss();

	if (FAILED(pInstance->Initialize(pBlackBoard, pDecorator)))
	{
		MSG_BOX("Failed to Created CSelector_Boss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSelector_Boss::Free()
{
	__super::Free();
}