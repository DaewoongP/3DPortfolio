#include "..\Public\Bullet.h"
#include "GameInstance.h"
#include "BulletTrail.h"

CBullet::CBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CBullet::CBullet(const CBullet& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBullet::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBullet::Initialize(void* pArg)
{
	CTransform::TRANSFORMDESC TransformDesc = CTransform::TRANSFORMDESC(50.0, XMConvertToRadians(0.0f));
	if (FAILED(__super::Initialize(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_eGameEvent = GAME_NOEVENT;

	m_vEmissiveColor = _float4(1.f, 0.1f, 0.1f, 1.f);

	return S_OK;
}

void CBullet::Tick(_double dTimeDelta)
{
	m_pTransformCom->Go_Straight(dTimeDelta);

	__super::Tick(dTimeDelta);

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	m_pModelCom->Play_Animation(dTimeDelta);
}

GAMEEVENT CBullet::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	m_dDeleteTimeAcc += dTimeDelta;
	// 총알 사망 삭제 시 콜라이더나 렌더러에 넣지 않음.
	if (true == m_isDead ||
		m_dDeleteTimeAcc > m_dDeleteTime)
	{
		m_eGameEvent = GAME_OBJECT_DEAD;
		return m_eGameEvent;
	}

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	pGameInstance->Add_Collider(m_eColType, m_pColliderCom);

	Safe_Release(pGameInstance);

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pBulletTrail->Render_Effect();
#ifdef _DEBUG
		m_pColliderCom->Set_Color(DirectX::Colors::Red);
		m_pRendererCom->Add_DebugGroup(m_pColliderCom);
#endif // _DEBUG
	}

	return m_eGameEvent;
}

void CBullet::OnCollisionEnter(COLLISIONDESC CollisionDesc)
{
	if (COLLISIONDESC::COLTYPE_ENEMY == CollisionDesc.ColType ||
		COLLISIONDESC::COLTYPE_PLAYER == CollisionDesc.ColType)
		m_isDead = true;
}

HRESULT CBullet::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pShaderCom->Begin(2);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CBullet::Reset()
{
	if (FAILED(__super::Reset()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBullet::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Bullet"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		MSG_BOX("Failed CBullet Add_Component : (Com_Model)");
		return E_FAIL;
	}

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		MSG_BOX("Failed CEnemy Add_Component : (Com_Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSG_BOX("Failed CBullet Add_Component : (Com_Shader)");
		return E_FAIL;
	}

	CBounding_Sphere::BOUNDINGSPHEREDESC SphereDesc;

	SphereDesc.fRadius = 0.3f;
	SphereDesc.vPosition = _float3(0.f, 0.f, 0.f);

	/* For.Com_Collider */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &SphereDesc)))
	{
		MSG_BOX("Failed CEnemy_Pistol Add_Component : (Com_Collider)");
		return E_FAIL;
	}

	XMStoreFloat4x4(&m_TrailRightMatrix, XMMatrixTranslation(0.f, 0.5f, 0.f));
	XMStoreFloat4x4(&m_TrailLeftMatrix, XMMatrixTranslation(0.f, -0.5f, 0.f));

	CVIBuffer_Rect_Trail::TRAILDESC TrailDesc;
	ZEROMEM(&TrailDesc);
	TrailDesc.iTrailNum = 30;
	TrailDesc.fMinVertexDistance = 0.1f;
	TrailDesc.pHighLocalMatrix = &m_TrailRightMatrix;
	TrailDesc.pLowLocalMatrix = &m_TrailLeftMatrix;
	TrailDesc.pPivotMatrix = m_pModelCom->Get_PivotFloat4x4Ptr();
	TrailDesc.pWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();

	/* For.Com_BulletTrail */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_BulletTrail"),
		TEXT("Com_BulletTrail"), reinterpret_cast<CComponent**>(&m_pBulletTrail), &TrailDesc)))
	{
		MSG_BOX("Failed CEnemy_Pistol Add_Component : (Com_BulletTrail)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBullet::SetUp_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", pGameInstance->Get_CamFar(), sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vEmissive", &m_vEmissiveColor, sizeof(_float4))))
		return E_FAIL;

	_bool isNormal = false;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_isNormal", &isNormal, sizeof(_bool))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

void CBullet::Fire(_fvector vInitPosition, _fvector vTargetPosition)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vInitPosition);
	m_pTransformCom->LookAt(vTargetPosition);
	m_pBulletTrail->Reset_Trail();
}

CBullet* CBullet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBullet* pInstance = new CBullet(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBullet::Clone(void* pArg)
{
	CBullet* pInstance = new CBullet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CBullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBullet::Free()
{
	__super::Free();

	Safe_Release(m_pBulletTrail);

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pColliderCom);
}
