#include "..\Public\Terrain.h"

CTerrain::CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
}

CTerrain::CTerrain(const CTerrain& rhs)
{
}

HRESULT CTerrain::Initialize_Prototype()
{
    return E_NOTIMPL;
}

HRESULT CTerrain::Initialize(void* pArg)
{
    return E_NOTIMPL;
}

void CTerrain::Tick(_double dTimeDelta)
{
}

void CTerrain::Late_Tick(_double dTimeDelta)
{
}

HRESULT CTerrain::Render()
{
    return E_NOTIMPL;
}

HRESULT CTerrain::Add_Component()
{
    return E_NOTIMPL;
}

CTerrain* CTerrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    return nullptr;
}

CGameObject* CTerrain::Clone(void* pArg)
{
    return nullptr;
}

void CTerrain::Free()
{
}
