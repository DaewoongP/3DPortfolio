#include "..\Public\Axis.h"

CAxis::CAxis(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
	, m_eState(AXIS_END)
{
	ZEROMEM(&m_vOriginAxisScale);
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());
}

CAxis::CAxis(const CAxis& rhs)
	:CGameObject(rhs)
	, m_eState(rhs.m_eState)
	, m_fDistance(rhs.m_fDistance)
	, m_fUIAxisSize(rhs.m_fUIAxisSize)
	, m_fX(rhs.m_fX)
	, m_fY(rhs.m_fY)
	, m_WorldMatrix(rhs.m_WorldMatrix)
	, m_ViewMatrix(rhs.m_ViewMatrix)
	, m_ProjMatrix(rhs.m_ProjMatrix)
	, m_vOriginAxisScale(rhs.m_vOriginAxisScale)
{
}

HRESULT CAxis::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CAxis::Initialize(void* pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(pArg), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_eState = *static_cast<AXISSTATE*>(pArg);

	m_fUIAxisSize = 50.f;
	m_fX = 1200.f;
	m_fY = 80.f;

	m_fDistance = 3.f;
	m_vOriginAxisScale = _float3(0.5f, 0.5f, 0.5f);
	return S_OK;
}

void CAxis::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
}

void CAxis::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);
	
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CAxis::Render()
{
	if (!m_isRendering)
		return S_OK;

	Set_Position();

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);
	
	m_pShaderCom->Begin(0);

	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	return S_OK;
}

HRESULT CAxis::Add_Component()
{
	if (FAILED(__super::Add_Component(LEVEL_TOOL,
		TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_TOOL,
		TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_TOOL,
		TEXT("Prototype_Component_Shader_Nontex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	_float3 LinePoints[6] = {
		// X_Axis
		_float3(0.f, 0.f, 0.f), _float3(1.f, 0.f, 0.f),
		// Y_Axis
		_float3(0.f, 0.f, 0.f), _float3(0.f, 1.f, 0.f),
		// Z_Axis
		_float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 1.f)
	};

	if (FAILED(__super::Add_Component(LEVEL_TOOL,
		TEXT("Prototype_Component_VIBuffer_Line"),
		TEXT("Com_Line"), reinterpret_cast<CComponent**>(&m_pLineCom), &CVIBuffer_Line::Line_Desc(6, LinePoints))))
		return E_FAIL;

	return S_OK;
}

HRESULT CAxis::SetUp_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	return S_OK;
}

void CAxis::Set_Position()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	switch (m_eState)
	{
	case AXIS_ORIGIN:
		Set_Origin(pGameInstance);
		break;
	case AXIS_UI:
		Set_UI(pGameInstance);
		break;
	case AXIS_CENTER:
		Set_Center(pGameInstance);
		break;
	default:
		break;
	}

	Safe_Release(pGameInstance);
}

void CAxis::Set_Origin(CGameInstance* pGameInstance)
{
	m_pTransformCom->Set_Scale(m_vOriginAxisScale);

	m_WorldMatrix = *m_pTransformCom->Get_WorldFloat4x4();
	m_ViewMatrix = *pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
	m_ProjMatrix = *pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);
}

void CAxis::Set_UI(CGameInstance* pGameInstance)
{
	_float4x4  ProjMatrix;
	_vector vPos = XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f);
	vPos = XMVector3TransformCoord(vPos, pGameInstance->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW));

	m_pTransformCom->Set_Scale(_float3(m_fUIAxisSize, m_fUIAxisSize, m_fUIAxisSize));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	XMStoreFloat4x4(&ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, m_fUIAxisSize));
	
	m_WorldMatrix = *m_pTransformCom->Get_WorldFloat4x4();
	m_ViewMatrix = *pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
	m_ProjMatrix = ProjMatrix;
}

void CAxis::Set_Center(CGameInstance* pGameInstance)
{
	m_pTransformCom->Set_Scale(_float3(0.5f, 0.5f, 0.5f));
	_vector vCamPos = XMLoadFloat4(&pGameInstance->Get_CamPosition());
	_vector vCamLook = pGameInstance->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW).r[2];
	_vector vAxisPos = vCamPos + XMVector4Normalize(vCamLook) * m_fDistance;


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vAxisPos);

	m_WorldMatrix = *m_pTransformCom->Get_WorldFloat4x4();
	m_ViewMatrix = *pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
	m_ProjMatrix = *pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);
}

CAxis* CAxis::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAxis* pInstance = new CAxis(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CAxis");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CAxis::Clone(void* pArg)
{
	CAxis* pInstance = new CAxis(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CAxis");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAxis::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pLineCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTransformCom);
}
