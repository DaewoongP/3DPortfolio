#include "..\Public\BlinkTarget.h"
#include "GameInstance.h"
#include "Player.h"

CBlinkTarget::CBlinkTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CBlinkTarget::CBlinkTarget(const CBlinkTarget& rhs)
	: CUI(rhs)
{
}

HRESULT CBlinkTarget::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBlinkTarget::Initialize(void* pArg)
{
	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fSizeX = 64.f;
	m_fSizeY = 64.f;

	m_fX = 0.f;
	m_fY = 0.f;

	m_vColor = _float4(1.f, 0.1f, 0.1f, 1.f);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBlinkTarget::Initialize_Level(_uint iLevelIndex)
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

void CBlinkTarget::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
}

GAMEEVENT CBlinkTarget::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);
	
	if (nullptr != m_pRendererCom &&
		0 != m_pPlayer->Get_BlinkTarget().size())
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return GAME_NOEVENT;
}

HRESULT CBlinkTarget::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_matrix ViewMatrix = pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);
	_matrix ProjMatrix = pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);

	for (auto& pTarget : m_pPlayer->Get_BlinkTarget())
	{
		_vector vTargetPos = pTarget->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		_vector vTargetViewPosition = XMVector3TransformCoord(XMVectorSetY(vTargetPos, XMVectorGetY(vTargetPos) + 2.f), ViewMatrix);
		_vector vTargetProjPosition = XMVector3TransformCoord(vTargetViewPosition, ProjMatrix);
		
		_float3 vTrans;
		XMStoreFloat3(&vTrans, vTargetProjPosition);
		
		m_pTransformCom->Set_State(CTransform::STATE_POSITION,
			XMVectorSet(
				vTrans.x * 0.5f * g_iWinSizeX,
				vTrans.y * 0.5f * g_iWinSizeY,
				0.f,
				1.f));

		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
			return E_FAIL;

		m_pShaderCom->Begin(2);

		m_pVIBufferCom->Render();
	}

	Safe_Release(pGameInstance);

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBlinkTarget::Add_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		MSG_BOX("Failed CBlinkTarget Add_Component : (Com_Renderer)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		MSG_BOX("Failed CBlinkTarget Add_Component : (Com_Shader)");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
	{
		MSG_BOX("Failed CBlinkTarget Add_Component : (Com_VIBuffer)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Blink_Target"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX("Failed CBlinkTarget Add_Component : (Com_Texture)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBlinkTarget::SetUp_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

CBlinkTarget* CBlinkTarget::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBlinkTarget* pInstance = new CBlinkTarget(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBlinkTarget");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBlinkTarget::Clone(void* pArg)
{
	CBlinkTarget* pInstance = new CBlinkTarget(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CBlinkTarget");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBlinkTarget::Free()
{
	__super::Free();

	Safe_Release(m_pPlayer);
}
