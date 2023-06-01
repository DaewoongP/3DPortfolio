#include "Terrain.h"
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
    FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
    return S_OK;
}

HRESULT CTerrain::Initialize(void* pArg)
{
    FAILED_CHECK_RETURN(__super::Initialize(pArg), E_FAIL);
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

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
        m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CTerrain::Render()
{
    FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

    m_pShaderCom->Begin(0);
    FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

    return S_OK;
}


HRESULT CTerrain::RemakeTerrain(_uint iSizeX, _uint iSizeY)
{
    if (0 == iSizeX ||
        0 == iSizeY)
        return S_OK;
    if (FAILED(m_pTerrainCom->RemakeTerrain(iSizeX, iSizeY)))
        return E_FAIL;

    return S_OK;
}

HRESULT CTerrain::Add_Component()
{
    if (FAILED(__super::Add_Component(LEVEL_TOOL,
        TEXT("Prototype_Component_Renderer"),
        TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_TOOL,
        TEXT("Prototype_Component_Shader_VtxNorTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_TOOL,
        TEXT("Prototype_Component_VIBuffer_Terrain"),
        TEXT("Com_Terrain"), reinterpret_cast<CComponent**>(&m_pTerrainCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_TOOL,
        TEXT("Prototype_Component_Texture_Terrain"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CTerrain::SetUp_ShaderResources()
{
    _float4x4 WorldMatrix;
    XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
        return E_FAIL;
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTRANSFORMSTATE::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTRANSFORMSTATE::D3DTS_PROJ))))
        return E_FAIL;

    D3D11_RASTERIZER_DESC rasterizer;
    ZeroMemory(&rasterizer, sizeof rasterizer);
    rasterizer.CullMode = D3D11_CULL_NONE;
    if (m_bIsWireFrame)
        rasterizer.FillMode = D3D11_FILL_WIREFRAME;
    else
        rasterizer.FillMode = D3D11_FILL_SOLID;

    if (FAILED(m_pShaderCom->Bind_Rasterizer("g_Rasterizer", &rasterizer)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
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
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTerrainCom);
    Safe_Release(m_pRendererCom);
}
