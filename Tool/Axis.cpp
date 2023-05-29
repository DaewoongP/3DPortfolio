#include "pch.h"
#include "Axis.h"
#include "Renderer.h"
#include "Shader.h"
#include "VIBuffer_Line.h"
#include "ToolInstance.h"
#include "GameInstance.h"

CAxis::CAxis(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject(pDevice, pContext)
    , m_pToolInstance{ CToolInstance::GetInstance() }
{
    Safe_AddRef(m_pToolInstance);
}

CAxis::CAxis(const CAxis& rhs)
    :CGameObject(rhs)
    , m_pToolInstance(rhs.m_pToolInstance)
{
    Safe_AddRef(m_pToolInstance);
}

HRESULT CAxis::Initialize_Prototype()
{
    FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
    return S_OK;
}

HRESULT CAxis::Initialize(void* pArg)
{
    FAILED_CHECK_RETURN(__super::Initialize(pArg), E_FAIL);
    FAILED_CHECK_RETURN(Add_Components(), E_FAIL);

    return S_OK;
}

void CAxis::Tick(_double TimeDelta)
{
    __super::Tick(TimeDelta);
}

void CAxis::Late_Tick(_double TimeDelta)
{
    __super::Late_Tick(TimeDelta);

    if (nullptr != m_pRendererCom)
        m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::RENDER_UI, this);
}

HRESULT CAxis::Render()
{
    FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

    m_pShaderCom->Begin(0);

    FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
    return S_OK;
}

HRESULT CAxis::Add_Components()
{
    if (FAILED(__super::Add_Component(static_cast<_uint>(LEVELID::LEVEL_TOOL),
        TEXT("Prototype_Component_Renderer"),
        TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(static_cast<_uint>(LEVELID::LEVEL_TOOL),
        TEXT("Prototype_Component_Shader_Nontex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    _float3 LinePoints[6] = {
        // X_Axis
        _float3(0.f, 0.f, 0.f), _float3(1.f, 0.f, 0.f),
        // Y_Axis
        _float3(0.f, 0.f, 0.f), _float3(0.f, 1.f, 0.f),
        // Z_Axis
        _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 1.f)
    };

    if (FAILED(__super::Add_Component(static_cast<_uint>(LEVELID::LEVEL_TOOL),
        TEXT("Prototype_Component_VIBuffer_Line"),
        TEXT("Com_Line"), reinterpret_cast<CComponent**>(&m_pLineCom), &Engine::CVIBuffer_Line::Line_Desc(6, LinePoints))))
        return E_FAIL;

    return S_OK;
}

HRESULT CAxis::SetUp_ShaderResources()
{
    _float4x4 WorldMatrix;
    XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
        return E_FAIL;

    CGameInstance* pGameInstance = CGameInstance::GetInstance();

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTRANSFORMSTATE::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTRANSFORMSTATE::D3DTS_PROJ))))
        return E_FAIL;
    return S_OK;
}

CAxis* CAxis::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CAxis* pInstance = new CAxis(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created CBackGround");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CAxis::Clone(void* pArg)
{
    CAxis* pInstance = new CAxis(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned CBackGround");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CAxis::Free()
{
    __super::Free();
    Safe_Release(m_pToolInstance);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pLineCom);
    Safe_Release(m_pRendererCom);
}
