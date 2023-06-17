#include "..\Public\Dummy.h"
#include "GameInstance.h"

CDummy::CDummy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
	ZEROMEM(&m_vPreScale);
	ZEROMEM(&m_vPreRotation);
	ZEROMEM(&m_vPreTransform);
}

CDummy::CDummy(const CDummy& rhs)
	: CGameObject(rhs)
	, m_ObjectDesc(rhs.m_ObjectDesc)
	, m_vPreScale(rhs.m_vPreScale)
	, m_vPreRotation(rhs.m_vPreRotation)
	, m_vPreTransform(rhs.m_vPreTransform)
	, m_iPreAnimationIndex(rhs.m_iPreAnimationIndex)
	, m_dPreAnimationSpeed(rhs.m_dPreAnimationSpeed)
{
}

void CDummy::Free()
{
	__super::Free();
	Safe_Release(m_pTransformCom);
}
