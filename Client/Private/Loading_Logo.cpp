#include "../public/Loading_Logo.h"
#include "GameInstance.h"

CLoading_Logo::CLoading_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());
}

CLoading_Logo::CLoading_Logo(const CLoading_Logo& rhs)
	: CGameObject(rhs)
{
}

HRESULT CLoading_Logo::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoading_Logo::Initialize(void* pArg)
{
	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Change_TexCoord(720.f, 0.f, 720.f, 300.f, m_pTextureCom, 0)))
	{
		MSG_BOX("Failed Change TexCoord");
		return E_FAIL;
	}

	// 윈도우 창에 꽉채우게 설정함.
	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;

	// 윈도우창의 중간에 표시하게 설정.
	m_fX = g_iWinSizeX * 0.5f;
	m_fY = g_iWinSizeY * 0.5f;

	// 트랜스폼에 값 세팅.
	// offset값에 맞춰 세팅해줌.
	m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(
			m_fX - g_iWinSizeX * 0.5f,
			-m_fY + g_iWinSizeY * 0.5f,
			0.f,
			1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	// 직교 투영을 통해 처리.
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

	return S_OK;
}

void CLoading_Logo::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CLoading_Logo::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoading_Logo::Add_Components()
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

	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom))))
	{
		MSG_BOX("Failed BackGround Add_Component : (Com_Transform)");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect_Dynamic"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
	{
		MSG_BOX("Failed BackGround Add_Component : (Com_VIBuffer)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading_Logo"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX("Failed BackGround Add_Component : (Com_Texture)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoading_Logo::SetUp_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}

CLoading_Logo* CLoading_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLoading_Logo* pInstance = new CLoading_Logo(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CLoading_Logo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLoading_Logo::Clone(void* pArg)
{
	CLoading_Logo* pInstance = new CLoading_Logo(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CLoading_Logo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoading_Logo::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
