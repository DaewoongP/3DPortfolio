#include "..\Public\ColProp.h"
#include "GameInstance.h"

CColProp::CColProp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CProp(pDevice, pContext)
{
}

CColProp::CColProp(const CColProp& rhs)
	: CProp(rhs)
{
}

HRESULT CColProp::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CColProp::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		PROPDESC PropDesc;
		COLPROPDESC ColPropDesc = (*static_cast<COLPROPDESC*>(pArg));
		lstrcpy(PropDesc.pModelPrototypeTag, ColPropDesc.pModelPrototypeTag);
		PropDesc.vPosition = ColPropDesc.vPosition;
		PropDesc.vRotation = ColPropDesc.vRotation;
		PropDesc.vScale = ColPropDesc.vScale;

		if (FAILED(__super::Initialize(&PropDesc)))
			return E_FAIL;

		if (FAILED(Add_Components(ColPropDesc)))
			return E_FAIL;
	}

	return S_OK;
}

void CColProp::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

GAMEEVENT CColProp::Late_Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	pGameInstance->Add_Collider(CCollision_Manager::COLTYPE_STATIC, m_pColliderCom);

	Safe_Release(pGameInstance);
	__super::Late_Tick(TimeDelta);

	return m_eGameEvent;
}

void CColProp::OnCollisionEnter(COLLISIONDESC CollisionDesc)
{
	//cout << "Props Enter" << endl;
	//m_eGameEvent = GAME_OBJECT_DEAD;
}

void CColProp::OnCollisionStay(COLLISIONDESC CollisionDesc)
{
	//cout << "Props Stay" << endl;
}

void CColProp::OnCollisionExit(COLLISIONDESC CollisionDesc)
{
	//cout << "Props Exit" << endl;
}

HRESULT CColProp::Render()
{
	__super::Render();

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif // _DEBUG

	return S_OK;
}

HRESULT CColProp::Add_Components(COLPROPDESC ColDesc)
{
	_tchar pPrototypeName[MAX_STR] = TEXT("");
	switch (ColDesc.eColliderType)
	{
	case CCollider::TYPE_SPHERE:
		/* For.Com_Collider */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColDesc.SphereDesc)))
		{
			MSG_BOX("Failed CPlayer Add_Component : (Com_Collider)");
			return E_FAIL;
		}
		break;
	case CCollider::TYPE_AABB:
		/* For.Com_Collider */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
			TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColDesc.AABBDesc)))
		{
			MSG_BOX("Failed CPlayer Add_Component : (Com_Collider)");
			return E_FAIL;
		}
		break;
	case CCollider::TYPE_OBB:
		/* For.Com_Collider */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
			TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColDesc.OBBDesc)))
		{
			MSG_BOX("Failed CPlayer Add_Component : (Com_Collider)");
			return E_FAIL;
		}
		break;
	}

	return S_OK;
}

CColProp* CColProp::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CColProp* pInstance = new CColProp(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CColProp");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CColProp::Clone(void* pArg)
{
	CColProp* pInstance = new CColProp(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CColProp");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CColProp::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
}
