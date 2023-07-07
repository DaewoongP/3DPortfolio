#include "..\Public\BlackBoard.h"

CBlackBoard::CBlackBoard()
{
}

HRESULT CBlackBoard::Initialize()
{

	return S_OK;
}

HRESULT CBlackBoard::Add_Value(const _tchar* pValueTag, any pValue)
{
	m_Values.emplace(pValueTag, pValue);

	return S_OK;
}

any CBlackBoard::Find_Value(const _tchar* pValueTag)
{
	//return m_Values[pValueTag];
	auto iter = find_if(m_Values.begin(), m_Values.end(), CTag_Finder(pValueTag));

	if (iter == m_Values.end())
		return nullptr;

	return iter->second;
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
