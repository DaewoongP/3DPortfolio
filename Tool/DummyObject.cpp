#include "pch.h"
#include "DummyObject.h"
#include "GameInstance.h"
#include "ToolInstance.h"

CDummyObject::CDummyObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
	, m_pToolInstance{ CToolInstance::GetInstance() }
{
	Safe_AddRef(m_pToolInstance);


	m_matWorld = XMMatrixIdentity();
}

CDummyObject::CDummyObject(const CDummyObject& rhs)
	: CGameObject(rhs)
	, m_pToolInstance(rhs.m_pToolInstance)
	, m_matWorld(rhs.m_matWorld)
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

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::RENDER_BLEND, this);
}

HRESULT CDummyObject::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	m_pShaderCom->Begin(0);
	m_pCubeCom->Render();

	return S_OK;
}

HRESULT CDummyObject::Add_Components()
{
	if (FAILED(__super::Add_Component(static_cast<_uint>(LEVELID::LEVEL_STATIC),
		TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(static_cast<_uint>(LEVELID::LEVEL_STATIC),
		TEXT("Prototype_Component_Shader_Cube"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(static_cast<_uint>(LEVELID::LEVEL_STATIC),
		TEXT("Prototype_Component_VIBuffer_Cube"),
		TEXT("Com_Cube"), reinterpret_cast<CComponent**>(&m_pCubeCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CDummyObject::SetUp_ShaderResources()
{
	_vector vPick = m_pToolInstance->m_pDynamicCamera->Picking();
	memcpy(&m_matWorld.r[3], &vPick, sizeof _vector);
	if (FAILED(m_pShaderCom->Bind_WVPMatrix(m_matWorld * m_pToolInstance->m_pDynamicCamera->m_matCam)))
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
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pCubeCom);
	Safe_Release(m_pToolInstance);
}
