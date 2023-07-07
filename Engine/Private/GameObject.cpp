#include "..\Public\GameObject.h"
#include "GameInstance.h"

CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComposite(pDevice, pContext)
{
}

CGameObject::CGameObject(const CGameObject& rhs)
	: CComposite(rhs)
{
	m_pTransformCom = static_cast<CTransform*>(rhs.m_pTransformCom->Clone(nullptr));
}

HRESULT CGameObject::Initialize_Prototype()
{
	m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);

	if (nullptr == m_pTransformCom)
		return E_FAIL;
	return S_OK;
}

HRESULT CGameObject::Initialize(void* pArg, CTransform::TRANSFORMDESC* pTransformDesc)
{
	if (nullptr != pTransformDesc)
	{
		m_pTransformCom->Set_Desc(*pTransformDesc);
		m_pTransformCom->Set_Owner(this);

		m_Components.emplace(TEXT("Com_Transform"), m_pTransformCom);

		Safe_AddRef(m_pTransformCom);
	}

	return S_OK;
}

void CGameObject::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
}

GAMEEVENT CGameObject::Late_Tick(_double dTimeDelta)
{
	return __super::Late_Tick(dTimeDelta);
}

HRESULT CGameObject::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGameObject::Reset()
{
	if (FAILED(__super::Reset()))
		return E_FAIL;

	return S_OK;
}

void CGameObject::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
}
