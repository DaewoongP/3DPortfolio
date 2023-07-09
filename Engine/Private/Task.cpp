#include "..\Public\Task.h"

CTask::CTask()
{
}

HRESULT CTask::Initialize(CBlackBoard* pBlackBoard)
{
	// Task는 데코레이터 패스.
	if (FAILED(__super::Initialize(pBlackBoard, nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTask::Reset()
{
	if (FAILED(__super::Reset()))
		return E_FAIL;

	return S_OK;
}

void CTask::Free()
{
	__super::Free();
}
