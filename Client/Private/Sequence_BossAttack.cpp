#include "..\Public\Sequence_BossAttack.h"
#include "Sequence_Landing.h"
#include "Sequence_Charge.h"
#include "RandomChoose_AttackSequence.h"
#include "GameObject.h"

HRESULT CSequence_BossAttack::Initialize(CBlackBoard* pBlackBoard, CDecorator* pDecorator)
{
	if (FAILED(__super::Initialize(pBlackBoard, pDecorator)))
		return E_FAIL;

	if (FAILED(Add_Child(TEXT("Sequence_Landing"), CSequence_Landing::Create(pBlackBoard))))
		return E_FAIL;
	if (FAILED(Add_Child(TEXT("Sequence_Charge"), CSequence_Charge::Create(pBlackBoard))))
		return E_FAIL;
	if (FAILED(Add_Child(TEXT("RandomChoose_AttackSequence"), CRandomChoose_AttackSequence::Create(pBlackBoard))))
		return E_FAIL;

	m_Decorators.push_back(
		CDecorator::Create([&](CBlackBoard* pDecoBlackBoard)->_bool
			{
				if (true == *static_cast<_bool*>(m_pBlackBoard->Find_Value(TEXT("Value_isAttackFinished"))))
					return false;

				return true;
			})
	);

	return S_OK;
}

CSequence_BossAttack* CSequence_BossAttack::Create(CBlackBoard* pBlackBoard, CDecorator* pDecorator)
{
	CSequence_BossAttack* pInstance = new CSequence_BossAttack();

	if (FAILED(pInstance->Initialize(pBlackBoard, pDecorator)))
	{
		MSG_BOX("Failed to Created CSequence_BossAttack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSequence_BossAttack::Free()
{
	__super::Free();
}
