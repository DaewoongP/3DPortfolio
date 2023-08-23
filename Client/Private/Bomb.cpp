#include "..\Public\Bomb.h"
#include "GameInstance.h"
#include "ExplodeParticle.h"

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
	m_bCreatedRange = true;

	m_fExplodeSize = 50.f;
	m_dExplodeTime = 3.0;
	// 콜라이더를 띄우기 위한 시간.
	m_dDeleteTime = m_dExplodeTime + 1.5;

	CTransform::TRANSFORMDESC TransformDesc = CTransform::TRANSFORMDESC(100.0, XMConvertToRadians(0.0f));
	if (FAILED(__super::Initialize(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_eGameEvent = GAME_NOEVENT;
	
#ifdef _DEBUG
	m_pColliderCom->Set_Color(DirectX::Colors::Red);
#endif // _DEBUG
	
	return S_OK;
}

void CBomb::Tick(_double dTimeDelta)
{
	if (0.05f < XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
		m_pTransformCom->Go_Straight(dTimeDelta);

	__super::Tick(dTimeDelta);

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	if (1.f > XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
	{
		if (true == m_bCreatedRange)
		{
			m_pExplodeRange = CExplodeRange::Create(m_pDevice, m_pContext, m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_fExplodeSize, m_dExplodeTime);
			m_bCreatedRange = false;
		}

		m_dExplodeTimeAcc += dTimeDelta;
	}

	if (nullptr != m_pExplodeRange)
		m_pExplodeRange->Tick(dTimeDelta);
}

GAMEEVENT CBomb::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (nullptr != m_pExplodeRange)
		m_pExplodeRange->Late_Tick(dTimeDelta);
	
	if (m_dExplodeTimeAcc > m_dDeleteTime)
	{
		m_eGameEvent = GAME_OBJECT_DEAD;
		return m_eGameEvent;
	}
	if (m_dExplodeTimeAcc > m_dExplodeTime)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		if (false == m_isExplodeParticle)
		{
			m_isExplodeParticle = true;
			m_pExplodeParticle->Render_Effect(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			_vector vCamPos = XMLoadFloat4(pGameInstance->Get_CamPosition());
			_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_float fLength = XMVectorGetX(XMVector3Length(vCamPos - vPos));
			_float fSound;
			if (fLength != 0.f)
				fSound = 0.2f * (1 + 1 / fLength);
			else
				fSound = 0.5f;

			pGameInstance->Play_Sound(TEXT("Boss_BombExplosion%d.ogg"), 2, CSound_Manager::SOUND_ETC, fSound, true);
		}

		pGameInstance->Add_Collider(COLLISIONDESC::COLTYPE_ENEMYWEAPON, m_pColliderCom);

		Safe_Release(pGameInstance);
	}

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
#ifdef _DEBUG
		m_pRendererCom->Add_DebugGroup(m_pColliderCom);
#endif // _DEBUG
	}

	return m_eGameEvent;
}

HRESULT CBomb::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE);

		m_pShaderCom->Begin(3);

		m_pModelCom->Render(i);
	}

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

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_BOSS, TEXT("Prototype_Component_Model_Bomb"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSG_BOX("Failed CBoss_Sword Add_Component : (Com_Model)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSG_BOX("Failed CBoss_Sword Add_Component : (Com_Shader)");
		return E_FAIL;
	}

	CBounding_Sphere::BOUNDINGSPHEREDESC SphereDesc;

	SphereDesc.fRadius = m_fExplodeSize * 0.5f;
	SphereDesc.vPosition = _float3(0.f, 0.f, 0.f);

	/* For.Com_Collider */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &SphereDesc)))
	{
		MSG_BOX("Failed CEnemy_Pistol Add_Component : (Com_Collider)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(CComposite::Add_Component(LEVEL_BOSS, TEXT("Prototype_Component_Texture_Bomb_Emissive"),
		TEXT("Com_EmissiveTexture"), reinterpret_cast<CComponent**>(&m_pEmissiveTextureCom))))
	{
		MSG_BOX("Failed CBoss Add_Component : (Com_EmissiveTexture)");
		return E_FAIL;
	}
	
	/* For.Com_ExplodeParticle */
	if (FAILED(CComposite::Add_Component(LEVEL_BOSS, TEXT("Prototype_GameObject_ExplodeParticle"),
		TEXT("Com_ExplodeParticle"), reinterpret_cast<CComponent**>(&m_pExplodeParticle))))
	{
		MSG_BOX("Failed CBoss Add_Component : (Com_ExplodeParticle)");
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
	if (FAILED(m_pEmissiveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_EmissiveTexture")))
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

	Safe_Release(m_pExplodeRange);
	Safe_Release(m_pExplodeParticle);

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pEmissiveTextureCom);
}
