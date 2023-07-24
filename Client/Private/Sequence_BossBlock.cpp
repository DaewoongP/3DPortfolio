#include "..\Public\Sequence_BossBlock.h"

HRESULT CSequence_BossBlock::Initialize(CBlackBoard* pBlackBoard, CDecorator* pDecorator)
{
	if (FAILED(__super::Initialize(pBlackBoard, pDecorator)))
		return E_FAIL;



	return S_OK;
}

CSequence_BossBlock* CSequence_BossBlock::Create(CBlackBoard* pBlackBoard, CDecorator* pDecorator)
{
	CSequence_BossBlock* pInstance = new CSequence_BossBlock();

	if (FAILED(pInstance->Initialize(pBlackBoard, pDecorator)))
	{
		MSG_BOX("Failed to Created CSequence_BossBlock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSequence_BossBlock::Free()
{
	__super::Free();
}
