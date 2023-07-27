#include "..\Public\Indicator.h"
#include "GameInstance.h"
#include "Player.h"
#include "Layer.h"

CIndicator::CIndicator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CIndicator::CIndicator(const CIndicator& rhs)
	: CUI(rhs)
{
}

HRESULT CIndicator::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CIndicator::Initialize(void* pArg)
{
	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fSizeX = 128.f;
	m_fSizeY = 64.f;

	// 윈도우창의 중간에 표시하게 설정.
	m_fX = g_iWinSizeX * 0.5f;
	m_fY = g_iWinSizeY * 0.4f;

	m_fIndicatorRange = 20.f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CIndicator::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
}

GAMEEVENT CIndicator::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return GAME_NOEVENT;
}

HRESULT CIndicator::Render()
{
	// 절두체 컬링이 끝난 후 연산
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	
	_vector vCamPos = XMVectorSetY(XMLoadFloat4(pGameInstance->Get_CamPosition()), 0.f);
	_float4 vCamLook;
	memcpy(&vCamLook, &(pGameInstance->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_VIEW)->m[2]), sizeof(_float4));

	for (auto& vPosition : pGameInstance->Get_CulledPositions())
	{
		_vector vPos = XMVectorSetY(XMLoadFloat4(&vPosition), 0.f);
		if (m_fIndicatorRange < XMVectorGetX(XMVector3Length(vPos - vCamPos)))
			continue;
		_vector vIndicatorDir = XMVector3Normalize(vPos - vCamPos);

		// Rendering
		if (FAILED(SetUp_ShaderResources()))
			return E_FAIL;

		// 화면에 위치를 뽑기위한 회전
		_float fRad = acosf(XMVectorGetX(XMVector3Dot(vIndicatorDir, XMVector3Normalize(XMLoadFloat4(&vCamLook)))));
		if (0 > XMVectorGetY(XMVector3Cross(vIndicatorDir, XMLoadFloat4(&vCamLook))))
			fRad *= -1.f;
		
		_float4x4 WorldMatrix;
		XMStoreFloat4x4(&WorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMMatrixRotationZ(fRad));

		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
			return E_FAIL;

		m_pShaderCom->Begin(1);

		m_pVIBufferCom->Render();
	}

	Safe_Release(pGameInstance);

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CIndicator::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Indicator"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX("Failed BackGround Add_Component : (Com_Texture)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CIndicator::SetUp_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}

CIndicator* CIndicator::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CIndicator* pInstance = new CIndicator(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CIndicator");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CIndicator::Clone(void* pArg)
{
	CIndicator* pInstance = new CIndicator(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CIndicator");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIndicator::Free()
{
	__super::Free();
}