#include "..\Public\Task.h"

CTask::CTask()
{
}

HRESULT CTask::Initialize(CBlackBoard* pBlackBoard)
{
	if (FAILED(__super::Initialize(pBlackBoard)))
		return E_FAIL;

	return S_OK;
}

void CTask::Free()
{
	__super::Free();
}
