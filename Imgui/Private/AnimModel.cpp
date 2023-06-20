#include "..\Public\AnimModel.h"
#include "Animation.h"
#include "GameInstance.h"

CAnimModel::CAnimModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CDummy(pDevice, pContext)
{
}

CAnimModel::CAnimModel(const CAnimModel& rhs)
	: CDummy(rhs)
{
}

const _tchar* CAnimModel::Get_AnimationName() const
{
    if (nullptr == m_pModelCom)
        return 0;
    return m_pModelCom->Get_AnimationName();
}

_uint CAnimModel::Get_NumAnimations()
{
    if (nullptr == m_pModelCom)
        return 0;
    return m_pModelCom->Get_NumAnimations();
}

_uint CAnimModel::Get_MaxKeyFrame()
{
    if (nullptr == m_pModelCom)
        return 0;
    return m_pModelCom->Get_MaxKeyFrame();
}

_uint CAnimModel::Get_CurrentMaxChannelKeyFrameIndex()
{
    if (nullptr == m_pModelCom)
        return 0;
    return m_pModelCom->Get_CurrentMaxChannelKeyFrameIndex();
}

void CAnimModel::Set_CurrentKeyFrameIndex(_uint iKeyFrameIndex)
{
    if (nullptr == m_pModelCom)
        return;
    return m_pModelCom->Set_CurrentKeyFrameIndex(iKeyFrameIndex);
}

void CAnimModel::Set_AnimationPause(_bool bIsPaused)
{
    if (nullptr == m_pModelCom)
        return;
    return m_pModelCom->Set_AnimationPause(bIsPaused);
}

void CAnimModel::Set_AnimIndex(_uint iIndex)
{
    if (nullptr == m_pModelCom)
        return;
    m_pModelCom->Set_AnimIndex(iIndex);
}

void CAnimModel::Set_AnimationSpeed(_double dTime)
{
    if (nullptr == m_pModelCom)
        return;
    m_pModelCom->Get_Animation()->Set_TickPerSecond(dTime);
}

void CAnimModel::Set_AnimationFrameSpeed(_uint iFrameIndex, _float fSpeed)
{
    if (nullptr == m_pModelCom)
        return;
    m_pModelCom->Set_FrameSpeed(iFrameIndex, fSpeed);
}

HRESULT CAnimModel::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CAnimModel::Initialize(void* pArg)
{
    if (nullptr != pArg)
        m_ObjectDesc = *(static_cast<OBJECTDESC*>(pArg));
    else
        return E_FAIL;

	FAILED_CHECK_RETURN(__super::Initialize(pArg), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(m_ObjectDesc), E_FAIL);
    
    
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_ObjectDesc.vPosition));
	return S_OK;
}

void CAnimModel::Tick(_double dTimeDelta)
{
    __super::Tick(dTimeDelta);
    m_pModelCom->Play_Animation(dTimeDelta);
}

void CAnimModel::Late_Tick(_double dTimeDelta)
{
    __super::Late_Tick(dTimeDelta);
    if (nullptr != m_pRendererCom)
        m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CAnimModel::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    if (FAILED(SetUp_ShaderResources()))
        return E_FAIL;

    _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

        m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE);
        // m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS);

        m_pShaderCom->Begin(0);

        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CAnimModel::Add_Component(OBJECTDESC ObjectDesc)
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
        TEXT("Prototype_Component_Shader_AnimModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_TOOL,
        ObjectDesc.pModelPrototypeTag,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

	return S_OK;
}

HRESULT CAnimModel::SetUp_ShaderResources()
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    Safe_Release(pGameInstance);

	return S_OK;
}

CAnimModel* CAnimModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CAnimModel* pInstance = New CAnimModel(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created CAnimModel");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CAnimModel::Clone(void* pArg)
{
    CAnimModel* pInstance = New CAnimModel(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned CAnimModel");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CAnimModel::Free()
{
    __super::Free();
    Safe_Release(m_pRendererCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    //Safe_Release(m_pColliderCom);
    //Safe_Release(m_pTextureCom);
}
