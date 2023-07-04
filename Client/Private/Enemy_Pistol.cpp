#include "..\Public\Enemy_Pistol.h"
#include "GameInstance.h"
CEnemy_Pistol::CEnemy_Pistol(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEnemy(pDevice, pContext)
{
}

CEnemy_Pistol::CEnemy_Pistol(const CEnemy_Pistol& rhs)
	: CEnemy(rhs)
{
}

HRESULT CEnemy_Pistol::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnemy_Pistol::Initialize(void* pArg)
{
	ENEMYDESC EnemyDesc;
	ZEROMEM(&EnemyDesc);

	if (nullptr != pArg)
		EnemyDesc = *(static_cast<ENEMYDESC*>(pArg));
	else
	{
		MSG_BOX("Failed Read EnemyDesc");
		return E_FAIL;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(EnemyDesc)))
		return E_FAIL;

	m_pTransformCom->Set_Scale(EnemyDesc.vScale);
	m_pTransformCom->Rotation(EnemyDesc.vRotation);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&EnemyDesc.vPosition));

	m_pModelCom->Set_AnimIndex(101 + rand() % 5);

	return S_OK;
}

void CEnemy_Pistol::Tick(_double dTimeDelta)
{
	m_pModelCom->Play_Animation(dTimeDelta);

	__super::Tick(dTimeDelta);
	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
}

GAMEEVENT CEnemy_Pistol::Late_Tick(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	pGameInstance->Add_Collider(CCollision_Manager::COLTYPE_DYNAMIC, m_pColliderCom);

	Safe_Release(pGameInstance);
	__super::Late_Tick(dTimeDelta);

	return GAME_NOEVENT;
}

void CEnemy_Pistol::OnCollisionEnter(COLLISIONDESC CollisionDesc)
{
}

void CEnemy_Pistol::OnCollisionStay(COLLISIONDESC CollisionDesc)
{
}

void CEnemy_Pistol::OnCollisionExit(COLLISIONDESC CollisionDesc)
{
}

HRESULT CEnemy_Pistol::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE);
		// m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS);

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif // _DEBUG

	return S_OK;
}

HRESULT CEnemy_Pistol::Reset()
{
	if (FAILED(__super::Reset()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnemy_Pistol::Add_Component(ENEMYDESC& EnemyDesc)
{
	/* For.Com_Model */
	if (FAILED(CComposite::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Enemy_Pistol"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		MSG_BOX("Failed CEnemy_Pistol Add_Component : (Com_Model)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		MSG_BOX("Failed CEnemy_Pistol Add_Component : (Com_Shader)");
		return E_FAIL;
	}

	CBounding_AABB::BOUNDINGAABBDESC AABBDesc;

	AABBDesc.vExtents = _float3(1.f, 2.4f, 1.f);
	AABBDesc.vPosition = _float3(0.f, AABBDesc.vExtents.y, 0.f);

	/* For.Com_Collider */
	if (FAILED(CComposite::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &AABBDesc)))
	{
		MSG_BOX("Failed CEnemy_Pistol Add_Component : (Com_Collider)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEnemy_Pistol::Add_Parts()
{
	return S_OK;
}

HRESULT CEnemy_Pistol::SetUp_ShaderResources()
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

void CEnemy_Pistol::AnimationState(_double dTimeDelta)
{
}

void CEnemy_Pistol::Motion_Change(ANIMATIONFLAG eAnimationFlag)
{
}

CEnemy_Pistol* CEnemy_Pistol::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEnemy_Pistol* pInstance = new CEnemy_Pistol(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CEnemy_Pistol");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEnemy_Pistol::Clone(void* pArg)
{
	CEnemy_Pistol* pInstance = new CEnemy_Pistol(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CEnemy_Pistol");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEnemy_Pistol::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
}
