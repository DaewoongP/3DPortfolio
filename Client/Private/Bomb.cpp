#include "..\Public\Bomb.h"
#include "GameInstance.h"

CBomb::CBomb(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CBomb::CBomb(const CBomb& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBomb::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBomb::Initialize(void* pArg)
{
	CTransform::TRANSFORMDESC TransformDesc = CTransform::TRANSFORMDESC(100.0, XMConvertToRadians(0.0f));
	if (FAILED(__super::Initialize(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_eGameEvent = GAME_NOEVENT;

	return S_OK;
}

void CBomb::Tick(_double dTimeDelta)
{
	if (0.05f < XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
		m_pTransformCom->Go_Straight(dTimeDelta);

	__super::Tick(dTimeDelta);

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
}

GAMEEVENT CBomb::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (nullptr != m_pRendererCom)
	{
#ifdef _DEBUG
		m_pColliderCom->Set_Color(DirectX::Colors::Red);
		m_pRendererCom->Add_DebugGroup(m_pColliderCom);
#endif // _DEBUG
	}

	if (1.f > XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
		m_dExplodeTimeAcc += dTimeDelta;

	if (m_dExplodeTimeAcc > m_dDeleteTime)
	{
		m_eGameEvent = GAME_OBJECT_DEAD;
		return m_eGameEvent;
	}
	else if (m_dExplodeTimeAcc > m_dExplodeTime)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		pGameInstance->Add_Collider(COLLISIONDESC::COLTYPE_ENEMYWEAPON, m_pColliderCom);

		Safe_Release(pGameInstance);
	}

	return m_eGameEvent;
}

HRESULT CBomb::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBomb::Reset()
{
	if (FAILED(__super::Reset()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBomb::Add_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		MSG_BOX("Failed CEnemy Add_Component : (Com_Renderer)");
		return E_FAIL;
	}

	CBounding_Sphere::BOUNDINGSPHEREDESC SphereDesc;

	SphereDesc.fRadius = 20.f;
	SphereDesc.vPosition = _float3(0.f, 0.f, 0.f);

	/* For.Com_Collider */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &SphereDesc)))
	{
		MSG_BOX("Failed CEnemy_Pistol Add_Component : (Com_Collider)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBomb::SetUp_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

void CBomb::Fire(_fvector vInitPosition, _fvector vTargetPosition)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vInitPosition);
	m_pTransformCom->LookAt(vTargetPosition);
}

CBomb* CBomb::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBomb* pInstance = new CBomb(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBomb");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBomb::Clone(void* pArg)
{
	CBomb* pInstance = new CBomb(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CBomb");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBomb::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pColliderCom);
}
