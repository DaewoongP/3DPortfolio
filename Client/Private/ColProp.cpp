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

		if (FAILED(Add_Components()))
			return E_FAIL;

		if (FAILED(SetUp_Collider(ColPropDesc.szColliderFilePath)))
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
	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	return __super::Late_Tick(TimeDelta);
}

HRESULT CColProp::Render()
{
	__super::Render();

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif // _DEBUG

	return S_OK;
}

HRESULT CColProp::Add_Components()
{
	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom))))
	{
		MSG_BOX("Failed CPlayer Add_Component : (Com_Collider)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CColProp::SetUp_Collider(const _tchar* pColliderFilePath)
{
	HANDLE hFile = CreateFile(pColliderFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong	dwByte = 0;

	CCollider::TYPE eType;
	// Current Animation Index
	ReadFile(hFile, &eType, sizeof(CCollider::TYPE), &dwByte, nullptr);

	switch (eType)
	{
	case CCollider::TYPE_SPHERE:
		CBounding_Sphere::BOUNDINGSPHEREDESC SphereDesc;
		ReadFile(hFile, &SphereDesc, sizeof(CBounding_Sphere::BOUNDINGSPHEREDESC), &dwByte, nullptr);
		m_pColliderCom->Set_BoundingDesc(&SphereDesc);
		break;
	case CCollider::TYPE_AABB:
		CBounding_AABB::BOUNDINGAABBDESC AABBDesc;
		ReadFile(hFile, &AABBDesc, sizeof(CBounding_AABB::BOUNDINGAABBDESC), &dwByte, nullptr);
		m_pColliderCom->Set_BoundingDesc(&AABBDesc);
		break;
	case CCollider::TYPE_OBB:
		CBounding_OBB::BOUNDINGOBBDESC OBBDesc;
		ReadFile(hFile, &OBBDesc, sizeof(CBounding_OBB::BOUNDINGOBBDESC), &dwByte, nullptr);
		m_pColliderCom->Set_BoundingDesc(&OBBDesc);
		break;
	default:
		return E_FAIL;
	}

	CloseHandle(hFile);

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
