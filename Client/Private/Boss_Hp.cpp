#include "..\Public\Boss_Hp.h"
#include "GameInstance.h"
#include "Boss.h"

CBoss_Hp::CBoss_Hp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CBoss_Hp::CBoss_Hp(const CBoss_Hp& rhs)
	: CUI(rhs)
{
}

HRESULT CBoss_Hp::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Hp::Initialize(void* pArg)
{
	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fSizeX = g_iWinSizeX * 0.5f;
	m_fSizeY = 30.f;

	m_fX = g_iWinSizeX * 0.5f;
	m_fY = g_iWinSizeY * 0.1f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Hp::Initialize_Level(_uint iLevelIndex)
{
	if (FAILED(__super::Initialize_Level(iLevelIndex)))
		return E_FAIL;

	if (LEVEL_BOSS != iLevelIndex)
	{
		MSG_BOX("Level is Not BossLevel");
		return E_FAIL;
	}

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CGameObject* pGameObject = pGameInstance->Find_GameObject(iLevelIndex, TEXT("Layer_Boss"), TEXT("GameObject_Boss"));

	if (nullptr == pGameObject)
	{
		MSG_BOX("Failed Find Boss");
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	m_pBoss = dynamic_cast<CBoss*>(pGameObject);

	Safe_AddRef(m_pBoss);

	Safe_Release(pGameInstance);

	return S_OK;
}

void CBoss_Hp::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
}

GAMEEVENT CBoss_Hp::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return GAME_NOEVENT;
}

HRESULT CBoss_Hp::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;
	// HP
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	_float fHpPercent = m_pBoss->Get_HpPercent();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fUVPercentX", &fHpPercent, sizeof(_float))))
		return E_FAIL;

	m_pShaderCom->Begin(4);

	m_pVIBufferCom->Render();

	_matrix ScailedMatrix = XMMatrixScaling(1.017f, 1.f, 1.f) * m_pTransformCom->Get_WorldMatrix();
	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, ScailedMatrix);
	//HP Bar
	if (FAILED(m_pBarTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;

	m_pShaderCom->Begin(1);

	m_pVIBufferCom->Render();

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Hp::Add_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		MSG_BOX("Failed BackGround Add_Component : (Com_Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		MSG_BOX("Failed BackGround Add_Component : (Com_Shader)");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect_Dynamic"),
		TEXT("Com_DynamicVIBuffer"), reinterpret_cast<CComponent**>(&m_pDynamicBufferCom))))
	{
		MSG_BOX("Failed BackGround Add_Component : (Com_DynamicVIBuffer)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boss_Hp"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX("Failed BackGround Add_Component : (Com_Texture)");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
	{
		MSG_BOX("Failed BackGround Add_Component : (Com_VIBuffer)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boss_HpBar"),
		TEXT("Com_BarTexture"), reinterpret_cast<CComponent**>(&m_pBarTextureCom))))
	{
		MSG_BOX("Failed BackGround Add_Component : (Com_BarTexture)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBoss_Hp::SetUp_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	return S_OK;
}

CBoss_Hp* CBoss_Hp::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBoss_Hp* pInstance = new CBoss_Hp(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBoss_Hp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBoss_Hp::Clone(void* pArg)
{
	CBoss_Hp* pInstance = new CBoss_Hp(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CBoss_Hp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_Hp::Free()
{
	__super::Free();

	Safe_Release(m_pBoss);
	Safe_Release(m_pBarTextureCom);
	Safe_Release(m_pDynamicBufferCom);
}
