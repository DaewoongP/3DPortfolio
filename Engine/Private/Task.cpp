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

void CTask::Free()
{
	__super::Free();
}
