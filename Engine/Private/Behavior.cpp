#include "..\Public\Behavior.h"

CBehavior::CBehavior()
{
}

HRESULT CBehavior::Add_Child(CBehavior* pChild)
{
	if (nullptr == pChild)
		return E_FAIL;

	m_Childs.push_back(pChild);

	return S_OK;
}

HRESULT CBehavior::Initialize(CBlackBoard* pBlackBoard)
{
	m_pBlackBoard = pBlackBoard;

	return S_OK;
}

void CBehavior::Free()
{
	for (auto& pChild : m_Childs)
		Safe_Release(pChild);

	m_Childs.clear();

	Safe_Release(m_pBlackBoard);
}
