#include "..\Public\Katana.h"
#include "GameInstance.h"
#include "SwordTrail.h"
#include "Player.h"

CKatana::CKatana(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPart(pDevice, pContext)
{
}

CKatana::CKatana(const CKatana& rhs)
	: CPart(rhs)
{
}

HRESULT CKatana::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CKatana::Initialize(void* pArg)
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

HRESULT CKatana::Initialize_Level(_uint iLevelIndex)
{
	_uint iTrailFrontIndex = { 0 };
	_uint iTrailBackIndex = { 0 };

	if (FAILED(m_pModelCom->Find_BoneIndex(TEXT("Swoosh_Front"), &iTrailFrontIndex)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Find_BoneIndex(TEXT("Swoosh_Back"), &iTrailBackIndex)))
		return E_FAIL;

	CVIBuffer_Rect_Trail::TRAILDESC TrailDesc;
	ZEROMEM(&TrailDesc);
	TrailDesc.iTrailNum = 50;
	TrailDesc.fMinVertexDistance = 0.1f;
	TrailDesc.pHighLocalMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrixPtr(iTrailFrontIndex);
	TrailDesc.pLowLocalMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrixPtr(iTrailBackIndex);
	TrailDesc.pPivotMatrix = m_pModelCom->Get_PivotFloat4x4Ptr();
	TrailDesc.pWorldMatrix = &m_CombinedWorldMatrix;

	/* For.Com_Trail */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_SwordTrail"),
		TEXT("Com_Trail"), (CComponent**)&m_pSwordTrail, &TrailDesc)))
	{
		MSG_BOX("Failed CKatana Add_Component : (Com_Trail)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CKatana::Initialize_ParentMatrix(PARENTMATRIXDESC ParentDesc)
{
	m_ParentMatrixDesc = ParentDesc;
	
	return S_OK;
}

void CKatana::OnCollisionEnter(COLLISIONDESC CollisionDesc)
{
	if (COLLISIONDESC::COLTYPE_ENEMY == CollisionDesc.ColType)
		dynamic_cast<CPlayer*>(m_pOwner)->Gain_SkillStack();
}

void CKatana::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
	
	CBounding_AABB::BOUNDINGAABBDESC AABBDesc;
	_vector vLook = XMVectorSet(m_ParentMatrixDesc.pParentWorldMatrix->_31, m_ParentMatrixDesc.pParentWorldMatrix->_32, m_ParentMatrixDesc.pParentWorldMatrix->_33, 0.f);
	XMStoreFloat3(&AABBDesc.vPosition, vLook * 3.f);
	AABBDesc.vExtents = _float3(4.f, 4.f, 4.f);
	m_pColliderCom->Set_BoundingDesc(&AABBDesc);

	m_pColliderCom->Tick(XMLoadFloat4x4(&m_CombinedWorldMatrix));

	m_pModelCom->Play_Animation(dTimeDelta);
}

GAMEEVENT CKatana::Late_Tick(_double dTimeDelta)
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

HRESULT CKatana::Render()
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

HRESULT CKatana::Add_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSG_BOX("Failed CKatana Add_Component : (Com_Renderer)");
		return E_FAIL;
	}

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Katana"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSG_BOX("Failed CKatana Add_Component : (Com_Model)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSG_BOX("Failed CKatana Add_Component : (Com_Shader)");
		return E_FAIL;
	}

	CBounding_AABB::BOUNDINGAABBDESC AABBDesc;
	AABBDesc.vExtents = _float3(5.f, 5.f, 5.f);
	AABBDesc.vPosition = _float3(0.f, 0.f, 0.f);
	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &AABBDesc)))
	{
		MSG_BOX("Failed CPlayer Add_Component : (Com_Collider)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CKatana::SetUp_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;

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

void CKatana::Attack()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	pGameInstance->Add_Collider(COLLISIONDESC::COLTYPE_PLAYERWEAPON, m_pColliderCom);
	
	Safe_Release(pGameInstance);
}

void CKatana::Add_TrailRender()
{
	if (nullptr != m_pSwordTrail)
		m_pSwordTrail->Add_Render();
}

CKatana* CKatana::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CKatana* pInstance = new CKatana(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CKatana");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CKatana::Clone(void* pArg)
{
	CKatana* pInstance = new CKatana(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CKatana");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CKatana::Free()
{
	__super::Free();

	Safe_Release(m_pSwordTrail);

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pColliderCom);
}