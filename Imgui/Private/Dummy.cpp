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

void CDummy::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	if (nullptr != m_pColliderCom)
		m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
}

HRESULT CDummy::Render()
{
	__super::Render();

	return S_OK;
}

void CDummy::Set_BoundingDesc(CCollider::TYPE eType, void* pBoundingDesc)
{
	if (nullptr == pBoundingDesc)
		return;

	m_eColliderType = eType;

	switch (eType)
	{
	case Engine::CCollider::TYPE_SPHERE:
		m_pBoundingSphereDesc = *(CBounding_Sphere::BOUNDINGSPHEREDESC*)(pBoundingDesc);
		break;
	case Engine::CCollider::TYPE_AABB:
		m_pBoundingAABBDesc = *(CBounding_AABB::BOUNDINGAABBDESC*)(pBoundingDesc);
		break;
	case Engine::CCollider::TYPE_OBB:
		m_pBoundingOBBDesc = *(CBounding_OBB::BOUNDINGOBBDESC*)(pBoundingDesc);
		break;
	}

}

void CDummy::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
}
