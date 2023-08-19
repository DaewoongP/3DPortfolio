#include "../Public/MiniGame_Back.h"
#include "GameInstance.h"
#include "MiniGame_Manager.h"

CMiniGame_Back::CMiniGame_Back(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CMiniGame_Back::CMiniGame_Back(const CMiniGame_Back& rhs)
	: CUI(rhs)
{
}

HRESULT CMiniGame_Back::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMiniGame_Back::Initialize(void* pArg)
{
	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fSizeX = 1080.f;
	m_fSizeY = 128.f;

	m_fX = g_iWinSizeX * 0.5f;
	m_fY = g_iWinSizeY * 0.8f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	// z�� ����
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetZ(vPos, 0.1f));

	return S_OK;
}

void CMiniGame_Back::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
}

GAMEEVENT CMiniGame_Back::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	CMiniGame_Manager* pMiniGame_Manager = CMiniGame_Manager::GetInstance();
	Safe_AddRef(pMiniGame_Manager);

	if (nullptr != m_pRendererCom &&
		false == pMiniGame_Manager->IsFinished())
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	Safe_Release(pMiniGame_Manager);

	return GAME_NOEVENT;
}

HRESULT CMiniGame_Back::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);

	m_pVIBufferCom->Render();

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMiniGame_Back::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MiniBack"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX("Failed BackGround Add_Component : (Com_Texture)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMiniGame_Back::SetUp_ShaderResources()
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

CMiniGame_Back* CMiniGame_Back::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMiniGame_Back* pInstance = new CMiniGame_Back(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CMiniGame_Back");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMiniGame_Back::Clone(void* pArg)
{
	CMiniGame_Back* pInstance = new CMiniGame_Back(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CMiniGame_Back");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMiniGame_Back::Free()
{
	__super::Free();
}