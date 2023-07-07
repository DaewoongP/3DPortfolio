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

void CBehavior::Free()
{
}
