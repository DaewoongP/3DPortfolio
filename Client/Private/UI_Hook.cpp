#include "../Public/UI_Hook.h"
#include "GameInstance.h"
#include "Layer.h"
#include "ColProp.h"

CUI_Hook::CUI_Hook(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI_Hook::CUI_Hook(const CUI_Hook& rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI_Hook::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Hook::Initialize(void* pArg)
{
	CTransform::TRANSFORMDESC TransformDesc = CTransform::TRANSFORMDESC(0.0, XMConvertToRadians(0.0f));
	if (FAILED(__super::Initialize(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Hook::Initialize_Level(_uint iLevelIndex)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CLayer* pLayer = pGameInstance->Find_Layer(iLevelIndex, TEXT("Layer_Hook"));

	if (nullptr == pLayer)
		return E_FAIL;

	for (auto& pair : pLayer->Get_AllGameObject())
	{
		CColProp* pHook = dynamic_cast<CColProp*>(pair.second);

		if (nullptr == pHook)
			return E_FAIL;

		m_HookPositions.push_back(pHook->Get_Collider()->Get_BoundingCenterPosition());
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

void CUI_Hook::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
}

GAMEEVENT CUI_Hook::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_float4x4 WorldMatrix, ViewMatrix, BillMatrixY;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&BillMatrixY, XMMatrixIdentity());

	ViewMatrix = *pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);

	BillMatrixY._11 = ViewMatrix._11;
	BillMatrixY._13 = ViewMatrix._13;
	BillMatrixY._31 = ViewMatrix._31;
	BillMatrixY._33 = ViewMatrix._33;

	XMStoreFloat4x4(&WorldMatrix,
		XMMatrixScaling(5.f, 5.f, 5.f) *
		XMMatrixInverse(nullptr, XMLoadFloat4x4(&BillMatrixY)));

	vector<_float4x4>	InstanceMatricies;

	for (auto& vPosition : m_HookPositions)
	{
		_float4x4 InstanceMatrix;
		XMStoreFloat4x4(&InstanceMatrix, XMLoadFloat4x4(&WorldMatrix) * XMMatrixTranslation(vPosition.x, vPosition.y, vPosition.z));
		InstanceMatricies.push_back(InstanceMatrix);
	}

	Safe_Release(pGameInstance);

	m_pBufferCom->Tick(InstanceMatricies.data());

	return GAME_NOEVENT;
}

HRESULT CUI_Hook::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	// color, alphablend
	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Hook::Add_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		MSG_BOX("Failed BackGround Add_Component : (Com_Shader)");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point_Instance"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pBufferCom))))
	{
		MSG_BOX("Failed BackGround Add_Component : (Com_VIBuffer)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Hook"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX("Failed BackGround Add_Component : (Com_Texture)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_Hook::SetUp_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	_float4 vColor = _float4(0.f, 0.5f, 1.f, 1.f);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	return S_OK;
}

CUI_Hook* CUI_Hook::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Hook* pInstance = new CUI_Hook(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CUI_Hook");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Hook::Clone(void* pArg)
{
	CUI_Hook* pInstance = new CUI_Hook(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CUI_Hook");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Hook::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pRendererCom);
}
