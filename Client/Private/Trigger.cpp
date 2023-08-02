#include "..\Public\Trigger.h"
#include "GameInstance.h"

CTrigger::CTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CTrigger::CTrigger(const CTrigger& rhs)
	: CGameObject(rhs)
{
}

HRESULT CTrigger::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTrigger::Initialize(void* pArg)
{
	CTransform::TRANSFORMDESC TransformDesc;
	ZEROMEM(&TransformDesc);

	if (FAILED(__super::Initialize(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;
	
	return S_OK;
}

void CTrigger::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	pGameInstance->Add_Collider(COLLISIONDESC::COLTYPE_TRIGGER, m_pColliderCom);

	Safe_Release(pGameInstance);
}

GAMEEVENT CTrigger::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

#ifdef _DEBUG
	m_pRenderer->Add_DebugGroup(m_pColliderCom);
#endif // _DEBUG

	return GAME_NOEVENT;
}

void CTrigger::OnCollisionEnter(COLLISIONDESC CollisionDesc)
{
#ifdef _DEBUG
	cout << "Trigger Enter" << endl;
#endif // _DEBUG
}

void CTrigger::OnCollisionStay(COLLISIONDESC CollisionDesc)
{
}

void CTrigger::OnCollisionExit(COLLISIONDESC CollisionDesc)
{
#ifdef _DEBUG
	cout << "Trigger Exit" << endl;
#endif // _DEBUG
}

HRESULT CTrigger::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTrigger::Reset()
{
	if (FAILED(__super::Reset()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTrigger::Add_Component()
{
	CBounding_AABB::BOUNDINGAABBDESC AABBDesc;

	AABBDesc.vExtents = _float3(2.f, 2.f, 2.f);
	AABBDesc.vPosition = _float3(0.f, AABBDesc.vExtents.y, 0.f);

	/* For.Com_Collider */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &AABBDesc)))
	{
		MSG_BOX("Failed CTrigger Add_Component : (Com_Collider)");
		return E_FAIL;
	}

#ifdef _DEBUG
	/* For.Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("Failed CTrigger Add_Component : (Com_Renderer)");
		return E_FAIL;
	}
#endif // _DEBUG


	return S_OK;
}

void CTrigger::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	
#ifdef _DEBUG
	Safe_Release(m_pRenderer);
#endif // _DEBUG
}
