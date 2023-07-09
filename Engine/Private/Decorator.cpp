#include "..\Public\Decorator.h"

HRESULT CDecorator::Add_Decorator(function<_bool(class CBlackBoard*)> func)
{
	m_Decorators.push_back(func);

	return S_OK;
}

_bool CDecorator::Check_Decorator(class CBlackBoard* pBlackboard)
{
	for (auto& Decorator : m_Decorators)
	{
		if (false == Decorator(pBlackboard))
			return false;
	}

	return true;
}

CDecorator* CDecorator::Create(function<_bool(class CBlackBoard*)> func)
{
	CDecorator* pInstance = new CDecorator();

	if (FAILED(pInstance->Add_Decorator(func)))
	{
		MSG_BOX("Failed to Created CDecorator");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDecorator::Free()
{
}
