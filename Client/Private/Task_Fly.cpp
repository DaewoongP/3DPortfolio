#include "..\Public\Task_Fly.h"

HRESULT CTask_Fly::Initialize(CBlackBoard* pBlackBoard)
{
	if (FAILED(__super::Initialize(pBlackBoard)))
		return E_FAIL;

	return S_OK;
}

CBehavior::STATE CTask_Fly::Tick(_double dTimeDelta)
{
	return CBehavior::STATE();
}

HRESULT CTask_Fly::Reset()
{
	if (FAILED(__super::Reset()))
		return E_FAIL;

	return S_OK;
}

CTask_Fly* CTask_Fly::Create(CBlackBoard* pBlackBoard)
{
	CTask_Fly* pInstance = new CTask_Fly();

	if (FAILED(pInstance->Initialize(pBlackBoard)))
	{
		MSG_BOX("Failed to Created CTask_Fly");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTask_Fly::Free()
{
	__super::Free();
}
