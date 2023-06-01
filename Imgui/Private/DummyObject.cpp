#include "..\Public\DummyObject.h"
#include "GameInstance.h"

CDummyObject::CDummyObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CDummyObject::CDummyObject(const CDummyObject& rhs)
	: CGameObject(rhs)
{
}

HRESULT CDummyObject::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CDummyObject::Initialize(void* pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(pArg), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	return S_OK;
}

void CDummyObject::Tick(_double dTimeDelta)
{
    __super::Tick(dTimeDelta);
}

void CDummyObject::Late_Tick(_double dTimeDelta)
{
    __super::Late_Tick(dTimeDelta);
}

HRESULT CDummyObject::Render()
{
    FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);





	return S_OK;
}

HRESULT CDummyObject::Add_Component()
{
	return S_OK;
}

HRESULT CDummyObject::SetUp_ShaderResources()
{
	return S_OK;
}

CDummyObject* CDummyObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDummyObject* pInstance = new CDummyObject(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created CDummyObject");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CDummyObject::Clone(void* pArg)
{
    CDummyObject* pInstance = new CDummyObject(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned CDummyObject");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CDummyObject::Free()
{
    __super::Free();
    Safe_Release(m_pRendererCom);
    Safe_Release(m_pTransform);
    //Safe_Release(m_pMeshCom);
    Safe_Release(m_pShaderCom);
    //Safe_Release(m_pColliderCom);
    //Safe_Release(m_pTextureCom);
}
