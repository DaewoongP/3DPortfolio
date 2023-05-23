#include "pch.h"
#include "Axis.h"
#include "Renderer.h"
#include "Shader.h"
#include "VIBuffer_Line.h"

CAxis::CAxis(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject(pDevice, pContext)
{
}

CAxis::CAxis(const CAxis& rhs)
    :CGameObject(rhs)
{
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
        m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::RENDER_NONLIGHT, this);
}

HRESULT CAxis::Render()
{
    FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
    FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

    m_pShaderCom->Begin(0);
    m_pLineCom->Render();

    return S_OK;
}

HRESULT CAxis::Add_Components()
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
        TEXT("Prototype_Component_VIBuffer_Line"),
        TEXT("Com_Line"), reinterpret_cast<CComponent**>(&m_pLineCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CAxis::SetUp_ShaderResources()
{

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
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pLineCom);
    Safe_Release(m_pRendererCom);
}
