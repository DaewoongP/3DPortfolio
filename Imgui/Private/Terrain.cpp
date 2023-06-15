#include "Terrain.h"
#include "GameInstance.h"
#include "Engine_Defines.h"

CTerrain::CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CTerrain::CTerrain(const CTerrain& rhs)
    : CGameObject(rhs)
{
}

_uint CTerrain::Get_NumTextures()
{
    if (nullptr == m_pTextureCom)
        return 0;
    return m_pTextureCom->Get_NumTextures();
}

void CTerrain::Set_NumTexture(_int iIndex)
{
    m_iTextureIndex = iIndex;
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


HRESULT CTerrain::RemakeTerrain(const _tchar* pHeightMap)
{
    if (FAILED(m_pTerrainCom->RemakeTerrain(pHeightMap)))
        return E_FAIL;
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

HRESULT CTerrain::RemakeTerrain(_uint iTextureIndex)
{
    m_iTextureIndex = iTextureIndex;

    return S_OK;
}

HRESULT CTerrain::PickingOnTerrain(_Inout_ _float4* vPickPos)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    _float4 vRayPos, vRayDir;
    if (FAILED(pGameInstance->Get_MouseRay(m_pContext, g_hWnd, m_pTransformCom->Get_WorldMatrix_Inverse(), &vRayPos, &vRayDir)))
    {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }
    Safe_Release(pGameInstance);

    const _float3* vVertices =  m_pTerrainCom->Get_PosArray();
    const _uint* iIndex = m_pTerrainCom->Get_Index();
    _uint iNumIndices = m_pTerrainCom->Get_NumIndices();

    _float fIntersectsDistance = 9999.f;
    
    for (_uint i = 0; i < iNumIndices - 2; i++)
    {
        _float fDist;

        TriangleTests::Intersects(XMLoadFloat4(&vRayPos), XMVector3Normalize(XMLoadFloat4(&vRayDir)),
            XMLoadFloat3(&vVertices[iIndex[i]]),
            XMLoadFloat3(&vVertices[iIndex[i + 1]]),
            XMLoadFloat3(&vVertices[iIndex[i + 2]]),
            fDist);

        if (0.f < fDist && fIntersectsDistance > fDist)
        {
            fIntersectsDistance = fDist;
        }
    }
    if (fIntersectsDistance >= 9998.f)
        return E_FAIL;

    _vector vPos = XMLoadFloat4(&vRayPos) + (fIntersectsDistance * XMLoadFloat4(&vRayDir));
    XMStoreFloat4(vPickPos, vPos);

    return S_OK;
}

HRESULT CTerrain::Add_Component()
{
    if (FAILED(__super::Add_Component(LEVEL_TOOL,
        TEXT("Prototype_Component_Renderer"),
        TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_TOOL,
        TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_TOOL,
        TEXT("Prototype_Component_Shader_Terrain"),
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
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    Safe_Release(pGameInstance);

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_iTextureIndex)))
        return E_FAIL;
    return S_OK;
}

CTerrain* CTerrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTerrain* pInstance = New CTerrain(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created CTerrain");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CTerrain::Clone(void* pArg)
{
    CTerrain* pInstance = New CTerrain(*this);

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
    Safe_Release(m_pTransformCom);
    Safe_Release(m_pTerrainCom);
    Safe_Release(m_pRendererCom);
}
