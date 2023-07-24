#include "..\Public\Task_BossBlock.h"

HRESULT CTask_BossBlock::Initialize(CBlackBoard* pBlackBoard)
{
	if (FAILED(__super::Initialize(pBlackBoard)))
		return E_FAIL;

	return S_OK;
}

CBehavior::STATE CTask_BossBlock::Tick(_double dTimeDelta)
{
	return CBehavior::STATE();
}

HRESULT CTask_BossBlock::Reset()
{
	if (FAILED(__super::Reset()))
		return E_FAIL;

	return S_OK;
}

CTask_BossBlock* CTask_BossBlock::Create(CBlackBoard* pBlackBoard)
{
	CTask_BossBlock* pInstance = new CTask_BossBlock();

	if (FAILED(pInstance->Initialize(pBlackBoard)))
	{
		MSG_BOX("Failed to Created CTask_BossBlock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTask_BossBlock::Free()
{
	__super::Free();
}
