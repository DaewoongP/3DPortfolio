#include "..\Public\MiniGame_Cursor.h"
#include "GameInstance.h"

CMiniGame_Cursor::CMiniGame_Cursor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CMiniGame_Cursor::CMiniGame_Cursor(const CMiniGame_Cursor& rhs)
	: CUI(rhs)
	, m_pMiniGame_Manager(CMiniGame_Manager::GetInstance())
{
	Safe_AddRef(m_pMiniGame_Manager);
}

HRESULT CMiniGame_Cursor::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMiniGame_Cursor::Initialize(void* pArg)
{
	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fSizeX = 64.f;
	m_fSizeY = 64.f;

	m_fLeftX = g_iWinSizeX * 0.285f;
	m_fRightX = g_iWinSizeX * 0.77f;
	// 0.72 - 0.333f = 0.387
	// 0.387가지고 0~100 표현
	// 0.333f + percent * 0.387
	m_fX = m_fRightX;
	m_fY = g_iWinSizeY * 0.75f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetZ(vPos, 0.01f));

	return S_OK;
}

void CMiniGame_Cursor::Tick(_double dTimeDelta)
{
	m_fX = Get_WindowPixelX(_float(m_pMiniGame_Manager->Get_FirePercent()));

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSetX(vPos, m_fX - g_iWinSizeX * 0.5f));

	__super::Tick(dTimeDelta);
}

GAMEEVENT CMiniGame_Cursor::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return GAME_NOEVENT;
}

HRESULT CMiniGame_Cursor::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);

	m_pVIBufferCom->Render();

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMiniGame_Cursor::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
	{
		MSG_BOX("Failed BackGround Add_Component : (Com_VIBuffer)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Cursor"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX("Failed BackGround Add_Component : (Com_Texture)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMiniGame_Cursor::SetUp_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	return S_OK;
}

_float CMiniGame_Cursor::Get_WindowPixelX(_float fPercent)
{
	return m_fLeftX + fPercent * (m_fRightX - m_fLeftX);
}

CMiniGame_Cursor* CMiniGame_Cursor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMiniGame_Cursor* pInstance = new CMiniGame_Cursor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CMiniGame_Cursor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMiniGame_Cursor::Clone(void* pArg)
{
	CMiniGame_Cursor* pInstance = new CMiniGame_Cursor(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CMiniGame_Cursor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMiniGame_Cursor::Free()
{
	__super::Free();

	Safe_Release(m_pMiniGame_Manager);
}