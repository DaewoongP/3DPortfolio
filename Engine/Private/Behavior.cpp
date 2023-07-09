#include "..\Public\Behavior.h"

CBehavior::CBehavior()
{
}

HRESULT CBehavior::Add_Child(const _tchar* pChildTag, CBehavior* pChild)
{
	if (nullptr == pChild)
		return E_FAIL;

	if (nullptr != Find_Child(pChildTag))
	{
		MSG_BOX("Already Exist Child Tag");
		return E_FAIL;
	}
	
	m_Childs.push_back({ pChildTag, pChild });

	return S_OK;
}

HRESULT CBehavior::Add_Decorator(CDecorator* pDecorator)
{
	if (nullptr == pDecorator)
		return E_FAIL;

	m_Decorators.push_back(pDecorator);

	return S_OK;
}

HRESULT CBehavior::Initialize(CBlackBoard* pBlackBoard, CDecorator* pDecorator)
{
	m_pBlackBoard = pBlackBoard;

	if (nullptr != pDecorator)
		m_Decorators.push_back(pDecorator);

	return S_OK;
}

HRESULT CBehavior::Reset()
{
	m_iPreRunningChildIndex = -1;
	m_dRunningAcc = 0.0;

	return S_OK;
}

CBehavior* CBehavior::Find_Child(const _tchar* pChildTag)
{
	auto iter = find_if(m_Childs.begin(), m_Childs.end(), CTag_Finder(pChildTag));

	if (m_Childs.end() == iter)
		return nullptr;

	return iter->second;
}

void CBehavior::Free()
{
	for (auto& pChild : m_Childs)
		Safe_Release(pChild.second);
	m_Childs.clear();

	for (auto& pDecorator : m_Decorators)
		Safe_Release(pDecorator);
	m_Decorators.clear();

	Safe_Release(m_pBlackBoard);
}
