#include "Terrain.h"
#include "GameInstance.h"
#include "Cell.h"

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

void CTerrain::Set_TextureIndex(_int iIndex)
{
    m_iTextureIndex = iIndex;
}

HRESULT CTerrain::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CTerrain::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    m_Cells.clear();

    return S_OK;
}

void CTerrain::Tick(_double dTimeDelta)
{
    __super::Tick(dTimeDelta);

    if (0 < m_Cells.size())
    {
        vector<_float3> VertexPositions;

        _uint iCellIndex = { 0 };

        for (auto& pCell : m_Cells)
        {
            pCell->Tick(dTimeDelta);

            VertexPositions = pCell->Get_VertexPositions();

            for (size_t i = 0; i < VertexPositions.size(); ++i)
            {
                // 셀의 인덱스 번호에서 3개를 콜라이더를 만든다.
                m_CellColliders[iCellIndex * 3 + i]->Tick(XMMatrixTranslation(VertexPositions[i].x, VertexPositions[i].y, VertexPositions[i].z));
            }

            ++iCellIndex;
        }
    }

}

void CTerrain::Late_Tick(_double dTimeDelta)
{
    __super::Late_Tick(dTimeDelta);

    if (nullptr != m_pRendererCom)
        m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CTerrain::Render()
{
    if (FAILED(SetUp_ShaderResources()))
        return E_FAIL;
    
	m_pShaderCom->Begin(0);

    m_pTerrainCom->Render();

    m_pCellShaderCom->Begin(0);
   
    if (0 < m_Cells.size() &&
        0 < m_CellColliders.size())
    {
        for (auto& pCell : m_Cells)
        {
            pCell->Render();
        }

        for (auto& pCellCollider : m_CellColliders)
        {
            pCellCollider->Render();
        }
    }

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

HRESULT CTerrain::RemakeCells(vector<_float3*>& Cells)
{
    for (auto& pCell : m_Cells)
        Safe_Release(pCell);
    m_Cells.clear();
    // 재생성이므로 콜라이더도 재생성처리.
    for (auto& pCellCollider : m_CellColliders)
        Safe_Release(pCellCollider);
    m_CellColliders.clear();

    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    for (auto& pPoints : Cells)
    {
        // Cell Buffer Remake
        CVIBuffer_Cell* pCell = CVIBuffer_Cell::Create(m_pDevice, m_pContext, pPoints);

        if (nullptr == pCell)
            return E_FAIL;
        //  버텍스 위치가져와야함.
        m_Cells.push_back(pCell);
        
        // Cell Colliders Clone
        CBounding_Sphere::BOUNDINGSPHEREDESC	BoundingSphere;

        vector<_float3> VertexPositions = pCell->Get_VertexPositions();

        for (size_t i = 0; i < VertexPositions.size(); ++i)
        {
            BoundingSphere.vPosition = _float3(0.f, 0.f, 0.f);
            BoundingSphere.fRadius = 1.f;

            CComponent* pComponent = pGameInstance->Clone_Component(LEVEL_TOOL,
                TEXT("Prototype_Component_Collider_Sphere"), &BoundingSphere);

            if (nullptr == pComponent)
                return E_FAIL;

            m_CellColliders.push_back(static_cast<CCollider*>(pComponent));
        }
    }

    Safe_Release(pGameInstance);

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

    // 터레인 버퍼의 정점 배열
    const _float3* vVertices =  m_pTerrainCom->Get_PosArray();
    // 터레인의 인덱스 배열
    const _uint* iIndex = m_pTerrainCom->Get_Index();
    // 인덱스 개수
    _uint iNumIndices = m_pTerrainCom->Get_NumIndices();

    // 충돌 가능한 최대길이
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

_bool CTerrain::IsPickingOnCell(_float4* vPickPos)
{
    for (auto& pCollider : m_CellColliders)
    {
        
    }


    return false;
}

HRESULT CTerrain::Add_Component()
{
    if (FAILED(__super::Add_Component(LEVEL_TOOL,
        TEXT("Prototype_Component_Renderer"),
        TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
    {
        MSG_BOX("Failed CTerrain Add_Component : (Com_Renderer)");
        return E_FAIL;
    }

    if (FAILED(__super::Add_Component(LEVEL_TOOL,
        TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom))))
    {
        MSG_BOX("Failed CTerrain Add_Component : (Com_Transform)");
        return E_FAIL;
    }

    if (FAILED(__super::Add_Component(LEVEL_TOOL,
        TEXT("Prototype_Component_Shader_Terrain"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
    {
        MSG_BOX("Failed CTerrain Add_Component : (Com_Shader)");
        return E_FAIL;
    }

    if (FAILED(__super::Add_Component(LEVEL_TOOL,
        TEXT("Prototype_Component_VIBuffer_Terrain"),
        TEXT("Com_Terrain"), reinterpret_cast<CComponent**>(&m_pTerrainCom))))
    {
        MSG_BOX("Failed CTerrain Add_Component : (Com_Terrain)");
        return E_FAIL;
    }

    if (FAILED(__super::Add_Component(LEVEL_TOOL,
        TEXT("Prototype_Component_Texture_Terrain"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
    {
        MSG_BOX("Failed CTerrain Add_Component : (Com_Texture)");
        return E_FAIL;
    }

    if (FAILED(__super::Add_Component(LEVEL_TOOL,
        TEXT("Prototype_Component_Shader_Navigation"),
        TEXT("Com_NavigationShader"), reinterpret_cast<CComponent**>(&m_pCellShaderCom))))
    {
        MSG_BOX("Failed CTerrain Add_Component : (Com_NavigationShader)");
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

    _float4 vColor = _float4(0.f, 1.f, 0.f, 1.f);
    if (FAILED(m_pCellShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
        return E_FAIL;

    _float4x4 DebugMatrix = *m_pTransformCom->Get_WorldFloat4x4();
    // 네비게이션 메쉬렌더링을 깔끔하게 하기위한 유격
    DebugMatrix._42 += 0.001f;

    if (FAILED(m_pCellShaderCom->Bind_Matrix("g_WorldMatrix", &DebugMatrix)))
        return E_FAIL;

    if (FAILED(m_pCellShaderCom->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;

    if (FAILED(m_pCellShaderCom->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;
    
    Safe_Release(pGameInstance);

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

    Safe_Release(m_pCellShaderCom);

    for (auto& pCell : m_Cells)
        Safe_Release(pCell);
    m_Cells.clear();

    for (auto& pCellCollider : m_CellColliders)
        Safe_Release(pCellCollider);
    m_CellColliders.clear();

    Safe_Release(m_pTextureCom);;
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTransformCom);
    Safe_Release(m_pTerrainCom);
    Safe_Release(m_pRendererCom);
}