#include "..\Public\Task.h"

CTask::CTask()
{
}

HRESULT CTask::Initialize(CBlackBoard* pBlackBoard)
{
	// Task�� ���ڷ����� �н�.
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
