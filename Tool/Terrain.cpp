#include "pch.h"
#include "Terrain.h"
#include "GameInstance.h"
#include "ToolInstance.h"

CTerrain::CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
    , m_pToolInstance{ CToolInstance::GetInstance() }
{
    Safe_AddRef(m_pToolInstance);
}

CTerrain::CTerrain(const CTerrain& rhs)
    : CGameObject(rhs)
    , m_pToolInstance(rhs.m_pToolInstance)
{
    Safe_AddRef(m_pToolInstance);
}

HRESULT CTerrain::Initialize_Prototype()
{
    FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CTerrain::Initialize(void* pArg)
{
    FAILED_CHECK_RETURN(__super::Initialize(pArg), E_FAIL);
    FAILED_CHECK_RETURN(Add_Components(), E_FAIL);

    return S_OK;
}

void CTerrain::Tick(_double TimeDelta)
{
    __super::Tick(TimeDelta);
}

void CTerrain::Late_Tick(_double TimeDelta)
{
    __super::Late_Tick(TimeDelta);

    if (nullptr != m_pRendererCom)
        m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::RENDER_NONLIGHT, this);
}

HRESULT CTerrain::Render()
{
    FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
    FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);
    
    m_pShaderCom->Begin(0);
    m_pTerrainCom->Render();

    return S_OK;
}

HRESULT CTerrain::RemakeTerrain(_uint iSizeX, _uint iSizeY)
{
    if (FAILED(m_pTerrainCom->RemakeTerrain(iSizeX, iSizeY)))
        return E_FAIL;

    return S_OK;
}

HRESULT CTerrain::Add_Components()
{
    if (FAILED(__super::Add_Component(static_cast<_uint>(LEVELID::LEVEL_STATIC),
        TEXT("Prototype_Component_Renderer"),
        TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(static_cast<_uint>(LEVELID::LEVEL_STATIC),
        TEXT("Prototype_Component_Shader_Vtxtex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(static_cast<_uint>(LEVELID::LEVEL_STATIC),
        TEXT("Prototype_Component_VIBuffer_Terrain"),
        TEXT("Com_Terrain"), reinterpret_cast<CComponent**>(&m_pTerrainCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(static_cast<_uint>(LEVELID::LEVEL_STATIC),
        TEXT("Prototype_Component_Texture_Terrain"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CTerrain::SetUp_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_WVPMatrix(m_pToolInstance->m_pDynamicCamera->m_matCam)))
        return E_FAIL;
    D3D11_RASTERIZER_DESC rasterizer;
    ZeroMemory(&rasterizer, sizeof rasterizer);
    rasterizer.CullMode = D3D11_CULL_NONE;
    if (m_bIsWireFrame)
        rasterizer.FillMode = D3D11_FILL_WIREFRAME;
    else
        rasterizer.FillMode = D3D11_FILL_SOLID;

    if (FAILED(m_pShaderCom->Bind_Rasterizer(&rasterizer)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
        return E_FAIL;

    return S_OK;
}

CTerrain* CTerrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTerrain* pInstance = new CTerrain(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created CBackGround");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CTerrain::Clone(void* pArg)
{
    CTerrain* pInstance = new CTerrain(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned CBackGround");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CTerrain::Free()
{
    __super::Free();
    Safe_Release(m_pToolInstance);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTerrainCom);
    Safe_Release(m_pRendererCom);
}
