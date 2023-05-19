#include "pch.h"
#include "Terrain.h"
#include "Renderer.h"
#include "VIBuffer_RcCol.h"

CTerrain::CTerrain(LPDIRECT3DDEVICE9 pDevice)
    : CGameObject(pDevice)
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

    m_pRcColCom->Render();

    return S_OK;
}

HRESULT CTerrain::Add_Components()
{
    if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Renderer"),
        TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(TEXT("Prototype_Component_VIBuffer_RcCol"),
        TEXT("Com_RcCol"), reinterpret_cast<CComponent**>(&m_pRcColCom))))
        return E_FAIL;
    return S_OK;
}

CTerrain* CTerrain::Create(LPDIRECT3DDEVICE9 pDevice)
{
    CTerrain* pInstance = new CTerrain(pDevice);

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
    Safe_Release(m_pRcColCom);
    Safe_Release(m_pRendererCom);
}
