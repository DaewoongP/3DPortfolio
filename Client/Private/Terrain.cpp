#include "..\Public\Terrain.h"
#include "GameInstance.h"

CTerrain::CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CTerrain::CTerrain(const CTerrain& rhs)
    : CGameObject(rhs)
{
}

HRESULT CTerrain::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CTerrain::Initialize(void* pArg)
{
    CTransform::TRANSFORMDESC TransformDesc = CTransform::TRANSFORMDESC(0.0, XMConvertToRadians(0.0f));
    if (FAILED(__super::Initialize(&TransformDesc)))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    return S_OK;
}

void CTerrain::Tick(_double dTimeDelta)
{
    __super::Tick(dTimeDelta);
}

void CTerrain::Late_Tick(_double dTimeDelta)
{
    __super::Late_Tick(dTimeDelta);

    if (nullptr != m_pRendererCom)
        m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CTerrain::Render()
{
    FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

    m_pShaderCom->Begin(0);

    //m_pBufferCom->Render();
    
#ifdef _DEBUG
    m_pNavigationCom->Render();
#endif

    return S_OK;
}

HRESULT CTerrain::Add_Component()
{
    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pBufferCom))))
    {
        MSG_BOX("Failed CTerrain Add_Component : (Com_VIBuffer)");
        return E_FAIL;
    }
    // Test¿ë ÇÏÀÌÆ®¸Ê
    //m_pBufferCom->RemakeTerrain(TEXT("../../Resources/Terrain/Height.bmp"));

    /* For.Com_Renderer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
        TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
    {
        MSG_BOX("Failed CTerrain Add_Component : (Com_Renderer)");
        return E_FAIL;
    }

    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
    {
        MSG_BOX("Failed CTerrain Add_Component : (Com_Texture)");
        return E_FAIL;
    }

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Terrain"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
    {
        MSG_BOX("Failed CTerrain Add_Component : (Com_Shader)");
        return E_FAIL;
    }

    /* For.Com_Navigation*/
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
        TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom)))
    {
        MSG_BOX("Failed CTerrain Add_Component : (Com_Navigation)");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CTerrain::SetUp_ShaderResources()
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
        return E_FAIL;

    Safe_Release(pGameInstance);

    return S_OK;
}

CTerrain* CTerrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTerrain* pInstance = new CTerrain(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created CTerrain");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CTerrain::Clone(void* pArg)
{
    CTerrain* pInstance = new CTerrain(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned CTerrain");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CTerrain::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pBufferCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pRendererCom);
    Safe_Release(m_pNavigationCom);
}
