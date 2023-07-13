#include "..\Public\BlackBoard.h"

CBlackBoard::CBlackBoard()
{
}

HRESULT CBlackBoard::Initialize()
{

	return S_OK;
}

HRESULT CBlackBoard::Add_Value(const _tchar* pValueTag, void* pValue)
{
	m_Values.emplace(pValueTag, pValue);
	m_OriginalValues.emplace(pValueTag, pValue);

	return S_OK;
}

void* CBlackBoard::Find_Value(const _tchar* pValueTag)
{
	auto iter = find_if(m_Values.begin(), m_Values.end(), CTag_Finder(pValueTag));

	if (iter == m_Values.end())
		return nullptr;

	return iter->second;
}

HRESULT CBlackBoard::Reset()
{
	m_Values.clear();
	m_Values = m_OriginalValues;

	return S_OK;
}

CBlackBoard* CBlackBoard::Create()
{
	CBlackBoard* pInstance = new CBlackBoard();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created CBlackBoard");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBlackBoard::Free()
{
}
