#include "..\Public\Prop.h"
#include "GameInstance.h"

CProp::CProp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CProp::CProp(const CProp& rhs)
	: CGameObject(rhs)
{
}

HRESULT CProp::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CProp::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		m_PropDesc = *(static_cast<PROPDESC*>(pArg));
	}
	else
	{
		MSG_BOX("Failed Read PropDesc");
		return E_FAIL;
	}

	CTransform::TRANSFORMDESC TransformDesc = CTransform::TRANSFORMDESC(0.0, XMConvertToRadians(0.0f));
	if (FAILED(__super::Initialize(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(m_PropDesc.vScale);
	m_pTransformCom->Rotation(m_PropDesc.vRotation);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_PropDesc.vPosition));

	return S_OK;
}

HRESULT CProp::Initialize_Level(_uint iLevelIndex)
{
	if (FAILED(Add_Components_Level(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

void CProp::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

GAMEEVENT CProp::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	return GAME_NOEVENT;
}

HRESULT CProp::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;
	
	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
	_bool		isNormal = { false };

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE);
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TextureType_NORMALS)))
			isNormal = false;
		else
			isNormal = true;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_isNormal", &isNormal, sizeof(_bool))))
			return E_FAIL;

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CProp::Add_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSG_BOX("Failed CProp Add_Component : (Com_Shader)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CProp::Add_Components_Level(_uint iLevelIndex)
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(iLevelIndex, m_PropDesc.pModelPrototypeTag,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSG_BOX("Failed CProp Add_Component : (Com_Model)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CProp::SetUp_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
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

CProp* CProp::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CProp* pInstance = new CProp(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CProp");
		Safe_Release(pInstance);
	}
	return pInstance;
}


CGameObject* CProp::Clone(void* pArg)
{
	CProp* pInstance = new CProp(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CProp");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CProp::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
