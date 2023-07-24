#include "..\Public\Selector_BossPattern.h"
#include "Sequence_BossAttack.h"
#include "Sequence_BossBlock.h"

HRESULT CSelector_BossPattern::Initialize(CBlackBoard* pBlackBoard, CDecorator* pDecorator)
{
	if (FAILED(__super::Initialize(pBlackBoard, pDecorator)))
		return E_FAIL;

	if (FAILED(Add_Child(TEXT("Sequence_BossAttack"), CSequence_BossAttack::Create(pBlackBoard))))
		return E_FAIL;
	/*if (FAILED(Add_Child(TEXT("Sequence_BossBlock"), CSequence_BossBlock::Create(pBlackBoard))))
		return E_FAIL;*/

	return S_OK;
}

CSelector_BossPattern* CSelector_BossPattern::Create(CBlackBoard* pBlackBoard, CDecorator* pDecorator)
{
	CSelector_BossPattern* pInstance = new CSelector_BossPattern();

	if (FAILED(pInstance->Initialize(pBlackBoard, pDecorator)))
	{
		MSG_BOX("Failed to Created CSelector_BossPattern");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSelector_BossPattern::Free()
{
	__super::Free();
}
