#include "../Public/MainSlot.h"
#include "GameInstance.h"
#include "Player.h"

CMainSlot::CMainSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CMainSlot::CMainSlot(const CMainSlot& rhs)
	: CUI(rhs)
{
}

HRESULT CMainSlot::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainSlot::Initialize(void* pArg)
{
	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fSizeX = 64.f * 1.5f;
	m_fSizeY = 64.f * 1.5f;

	m_fX = g_iWinSizeX * 0.08f;
	m_fY = g_iWinSizeY * 0.9f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainSlot::Initialize_Level(_uint iLevelIndex)
{
	if (FAILED(__super::Initialize_Level(iLevelIndex)))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CGameObject* pGameObject = pGameInstance->Find_GameObject(iLevelIndex, TEXT("Layer_Player"), TEXT("GameObject_Player"));

	if (nullptr == pGameObject)
	{
		MSG_BOX("Failed Find Player");
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	m_pPlayer = dynamic_cast<CPlayer*>(pGameObject);
	Safe_AddRef(m_pPlayer);

	Safe_Release(pGameInstance);

	return S_OK;
}

void CMainSlot::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
}

GAMEEVENT CMainSlot::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return GAME_NOEVENT;
}

HRESULT CMainSlot::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(5);

	m_pVIBufferCom->Render();

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainSlot::Add_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		MSG_BOX("Failed CMainSlot Add_Component : (Com_Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		MSG_BOX("Failed CMainSlot Add_Component : (Com_Shader)");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
	{
		MSG_BOX("Failed CMainSlot Add_Component : (Com_VIBuffer)");
		return E_FAIL;
	}

	/* For.Com_BlinkTexture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Blink_Main"),
		TEXT("Com_BlinkTexture"), reinterpret_cast<CComponent**>(&m_pBlinkTexture))))
	{
		MSG_BOX("Failed CMainSlot Add_Component : (Com_BlinkTexture)");
		return E_FAIL;
	}

	/* For.Com_SurgeTexture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Surge_Main"),
		TEXT("Com_SurgeTexture"), reinterpret_cast<CComponent**>(&m_pSurgeTexture))))
	{
		MSG_BOX("Failed CMainSlot Add_Component : (Com_SurgeTexture)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMainSlot::SetUp_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (CPlayer::SKILL_BLINK == m_pPlayer->Get_CurrentSkill())
	{
		if (FAILED(m_pBlinkTexture->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
			return E_FAIL;
	}
	else if (CPlayer::SKILL_SURGE == m_pPlayer->Get_CurrentSkill())
	{
		if (FAILED(m_pSurgeTexture->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
			return E_FAIL;
	}

	_float4 vColor = _float4(1.f, 0.64705f, 0.f, 1.f);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
		return E_FAIL;

	_float fPercent = m_pPlayer->Get_SkillStackPercent();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fUVPercent", &fPercent, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CMainSlot* CMainSlot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMainSlot* pInstance = new CMainSlot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CMainSlot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMainSlot::Clone(void* pArg)
{
	CMainSlot* pInstance = new CMainSlot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CMainSlot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainSlot::Free()
{
	__super::Free();

	Safe_Release(m_pPlayer);
	Safe_Release(m_pBlinkTexture);
	Safe_Release(m_pSurgeTexture);
}
