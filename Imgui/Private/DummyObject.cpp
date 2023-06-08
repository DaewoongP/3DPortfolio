#include "..\Public\DummyObject.h"
#include "GameInstance.h"

CDummyObject::CDummyObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CDummyObject::CDummyObject(const CDummyObject& rhs)
	: CGameObject(rhs)
{
}

HRESULT CDummyObject::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CDummyObject::Initialize(void* pArg)
{
    OBJECTDESC ObjectDesc;
    if (nullptr != pArg)
        ObjectDesc = *(static_cast<OBJECTDESC*>(pArg));
    else
        return E_FAIL;

	FAILED_CHECK_RETURN(__super::Initialize(pArg), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(ObjectDesc), E_FAIL);
    
    
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&ObjectDesc.vPosition));
	return S_OK;
}

void CDummyObject::Tick(_double dTimeDelta)
{
    __super::Tick(dTimeDelta);
}

void CDummyObject::Late_Tick(_double dTimeDelta)
{
    __super::Late_Tick(dTimeDelta);
    if (nullptr != m_pRendererCom)
        m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CDummyObject::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    if (FAILED(SetUp_ShaderResources()))
        return E_FAIL;

    _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);
        // m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS);

        m_pShaderCom->Begin(0);

        m_pModelCom->Render(i);
    }

    return S_OK;
}

void CDummyObject::Move_Position(_fvector vPos, _double dTimeDelta)
{
    m_pTransformCom->Chase(vPos, dTimeDelta);
}

HRESULT CDummyObject::Add_Component(OBJECTDESC ObjectDesc)
{
    if (FAILED(__super::Add_Component(LEVEL_TOOL,
        TEXT("Prototype_Component_Renderer"),
        TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
        return E_FAIL;

    CTransform::TRANSFORMDESC TransformDesc;
    TransformDesc.dSpeedPerSec = 10.f;
    TransformDesc.dRotationPerSec = 5.f;
    if (FAILED(__super::Add_Component(LEVEL_TOOL,
        TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_TOOL,
        TEXT("Prototype_Component_Shader_DummyMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_TOOL,
        ObjectDesc.pModelPrototypeTag,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

	return S_OK;
}

HRESULT CDummyObject::SetUp_ShaderResources()
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

CDummyObject* CDummyObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDummyObject* pInstance = New CDummyObject(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created CDummyObject");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CDummyObject::Clone(void* pArg)
{
    CDummyObject* pInstance = New CDummyObject(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned CDummyObject");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CDummyObject::Free()
{
    __super::Free();
    Safe_Release(m_pBufferCom);
    Safe_Release(m_pRendererCom);
    Safe_Release(m_pTransformCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    //Safe_Release(m_pColliderCom);
    //Safe_Release(m_pTextureCom);
}
