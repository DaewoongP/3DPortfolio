#include "..\Public\Boss_Sword.h"
#include "GameInstance.h"
#include "Boss_SwordTrail.h"
#include "Boss.h"

CBoss_Sword::CBoss_Sword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPart(pDevice, pContext)
{
}

CBoss_Sword::CBoss_Sword(const CBoss_Sword& rhs)
	: CPart(rhs)
{
}

HRESULT CBoss_Sword::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Sword::Initialize(void* pArg)
{
	CTransform::TRANSFORMDESC TransformDesc = CTransform::TRANSFORMDESC(0.0, XMConvertToRadians(0.0f));
	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if (FAILED(Initialize_ParentMatrix(*reinterpret_cast<PARENTMATRIXDESC*>(pArg))))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Sword::Initialize_Level(_uint iLevelIndex)
{
	XMStoreFloat4x4(&m_SwordHighLocalMatrix, XMLoadFloat4x4(m_pModelCom->Get_PivotFloat4x4Ptr()) * XMMatrixTranslation(0.f, 0.f, 2.f));
	XMStoreFloat4x4(&m_SwordLowLocalMatrix, XMLoadFloat4x4(m_pModelCom->Get_PivotFloat4x4Ptr()) * XMMatrixTranslation(0.f, 0.f, 0.f));

	CVIBuffer_Rect_Trail::TRAILDESC TrailDesc;
	ZEROMEM(&TrailDesc);
	TrailDesc.iTrailNum = 20;
	TrailDesc.fMinVertexDistance = 0.1f;
	TrailDesc.pHighLocalMatrix = &m_SwordHighLocalMatrix;
	TrailDesc.pLowLocalMatrix = &m_SwordLowLocalMatrix;
	TrailDesc.pPivotMatrix = m_pModelCom->Get_PivotFloat4x4Ptr();
	TrailDesc.pWorldMatrix = &m_CombinedWorldMatrix;

	/* For.Com_Trail */
	if (FAILED(__super::Add_Component(LEVEL_BOSS, TEXT("Prototype_GameObject_Boss_SwordTrail"),
		TEXT("Com_Trail"), (CComponent**)&m_pSwordTrail, &TrailDesc)))
	{
		MSG_BOX("Failed CKatana Add_Component : (Com_Trail)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBoss_Sword::Initialize_ParentMatrix(PARENTMATRIXDESC ParentDesc)
{
	m_ParentMatrixDesc = ParentDesc;

	return S_OK;
}

void CBoss_Sword::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	m_pColliderCom->Tick(XMLoadFloat4x4(&m_CombinedWorldMatrix));

	m_pModelCom->Play_Animation(dTimeDelta);
}

GAMEEVENT CBoss_Sword::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
#ifdef _DEBUG
		m_pRendererCom->Add_DebugGroup(m_pColliderCom);
#endif // _DEBUG
	}

	return GAME_NOEVENT;
}

HRESULT CBoss_Sword::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE);
		m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TextureType_NORMALS);

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CBoss_Sword::Add_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSG_BOX("Failed CBoss_Sword Add_Component : (Com_Renderer)");
		return E_FAIL;
	}

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_BOSS, TEXT("Prototype_Component_Model_Bakunin_Sword"),
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

	CBounding_AABB::BOUNDINGAABBDESC AABBDesc;
	AABBDesc.vExtents = _float3(4.f, 4.f, 4.f);
	AABBDesc.vPosition = _float3(0.f, 0.f, 0.f);
	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &AABBDesc)))
	{
		MSG_BOX("Failed CBoss_Sword Add_Component : (Com_Collider)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBoss_Sword::SetUp_ShaderResources()
{
	// Part의 경우 월드행렬을 다 저장된거로 던져야함.
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix",
		pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix",
		pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", pGameInstance->Get_CamFar(), sizeof(_float))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

void CBoss_Sword::Attack()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	pGameInstance->Play_Sound(TEXT("Boss_Pattern.ogg"), CSound_Manager::SOUND_BOSS, 0.4f);

	pGameInstance->Add_Collider(COLLISIONDESC::COLTYPE_PARRYING, m_pColliderCom);

	Safe_Release(pGameInstance);
}

void CBoss_Sword::Add_TrailRender()
{
	m_pSwordTrail->Add_Render();
}

CBoss_Sword* CBoss_Sword::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBoss_Sword* pInstance = new CBoss_Sword(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBoss_Sword");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBoss_Sword::Clone(void* pArg)
{
	CBoss_Sword* pInstance = new CBoss_Sword(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CBoss_Sword");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_Sword::Free()
{
	__super::Free();

	Safe_Release(m_pSwordTrail);

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pColliderCom);
}