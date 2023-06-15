#include "..\Public\Dummy.h"
#include "GameInstance.h"

CDummy::CDummy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CDummy::CDummy(const CDummy& rhs)
	: CGameObject(rhs)
{
}

void CDummy::Free()
{
	__super::Free();
	Safe_Release(m_pTransformCom);
}
