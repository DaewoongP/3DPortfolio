#include "pch.h"
#include "DummyObject.h"
#include "GameInstance.h"
#include "ToolInstance.h"

CDummyObject::CDummyObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
	, m_pToolInstance{ CToolInstance::GetInstance() }
{
	Safe_AddRef(m_pToolInstance);
}

CDummyObject::CDummyObject(const CDummyObject& rhs)
	: CGameObject(rhs)
	, m_pToolInstance(rhs.m_pToolInstance)
{
	Safe_AddRef(m_pToolInstance);
}

HRESULT CDummyObject::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CDummyObject::Initialize(void* pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(pArg), E_FAIL);
	FAILED_CHECK_RETURN(Add_Components(), E_FAIL);

	return S_OK;
}

void CDummyObject::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
	
}

void CDummyObject::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);
	_vector vPick = m_pToolInstance->m_pFreeCam->Picking();
	m_pTransformCom->Set_State(CTransform::STATE::STATE_POSITION, vPick);
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::RENDER_BLEND, this);
}

HRESULT CDummyObject::Render()
{
	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	m_pShaderCom->Begin(0);
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	return S_OK;
}

HRESULT CDummyObject::Add_Components()
{
	if (FAILED(__super::Add_Component(static_cast<_uint>(LEVELID::LEVEL_TOOL),
		TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
		return E_FAIL;
	CTransform::TRANSFORMDESC TransformDesc;
	TransformDesc.dRotationPerSec = 3.f;
	TransformDesc.dSpeedPerSec = 10.f;
	if (FAILED(__super::Add_Component(static_cast<_uint>(LEVELID::LEVEL_TOOL),
		TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(static_cast<_uint>(LEVELID::LEVEL_TOOL),
		TEXT("Prototype_Component_Shader_Cube"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(static_cast<_uint>(LEVELID::LEVEL_TOOL),
		TEXT("Prototype_Component_VIBuffer_Cube"),
		TEXT("Com_Cube"), reinterpret_cast<CComponent**>(&m_pCubeCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CDummyObject::SetUp_ShaderResources()
{

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTRANSFORMSTATE::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTRANSFORMSTATE::D3DTS_PROJ))))
		return E_FAIL;

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
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pCubeCom);
	Safe_Release(m_pToolInstance);
}
