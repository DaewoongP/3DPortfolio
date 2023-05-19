#include "..\Public\Component_Manager.h"
#include "../Public/Component.h"

IMPLEMENT_SINGLETON(CComponent_Manager)

CComponent_Manager::CComponent_Manager()
{
}


HRESULT CComponent_Manager::Add_Prototype(const _tchar* pPrototypeTag, CComponent* pPrototype)
{
	if (nullptr != Find_Prototype(pPrototypeTag))
	{
		MSG_BOX("Already have Protorype In CComponent_Manager");
		return E_FAIL;
	}

	m_Prototypes.emplace(pPrototypeTag, pPrototype);

	return S_OK;
}

CComponent* CComponent_Manager::Clone_Component(const _tchar* pPrototypeTag, void* pArg)
{
	CComponent* pPrototype = Find_Prototype(pPrototypeTag);

	if (nullptr == pPrototype)
	{
		MSG_BOX("Failed Find Protorype");
		return nullptr;
	}
		

	CComponent* pComponent = pPrototype->Clone(pArg);
	if (nullptr == pComponent)
	{
		MSG_BOX("Failed Clone Protorype");
		return nullptr;
	}

	return pComponent;
}

CComponent* CComponent_Manager::Find_Prototype(const _tchar* pPrototypeTag)
{
	auto	iter = find_if(m_Prototypes.begin(), m_Prototypes.end(), CTag_Finder(pPrototypeTag));

	if (iter == m_Prototypes.end())
		return nullptr;

	return iter->second;
}

void CComponent_Manager::Free()
{
	for (auto& Pair : m_Prototypes)
		Safe_Release(Pair.second);
	m_Prototypes.clear();
}
