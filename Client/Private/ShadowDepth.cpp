#include "../Public/ShadowDepth.h"
#include "GameInstance.h"

CShadowDepth::CShadowDepth(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CShadowDepth::CShadowDepth(const CShadowDepth& rhs)
	: CGameObject(rhs)
{
}

HRESULT CShadowDepth::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CShadowDepth::Initialize(void* pArg)
{
	CTransform::TRANSFORMDESC TransformDesc = CTransform::TRANSFORMDESC(0.f, XMConvertToRadians(3.f));

	if (FAILED(__super::Initialize(pArg, &TransformDesc)))
		return E_FAIL;

	if (nullptr == pArg)
	{
		MSG_BOX("Depth Strcut NULL");
		return E_FAIL;
	}

	memcpy(&m_DepthDesc, pArg, sizeof(SHADOWDEPTH));
	
	return S_OK;
}

void CShadowDepth::Tick(_double dTimeDelta)
{
	if (IsInBoundingBox())
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		pGameInstance->Set_LightDepthMatrix(CPipeLine::D3DTS_VIEW, XMLoadFloat4x4(&m_DepthDesc.LightViewMatrix));
		pGameInstance->Set_LightDepthMatrix(CPipeLine::D3DTS_PROJ, XMLoadFloat4x4(&m_DepthDesc.LightProjMatrix));

		Safe_Release(pGameInstance);
	}

	__super::Tick(dTimeDelta);
}

GAMEEVENT CShadowDepth::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	return GAME_NOEVENT;
}

_bool CShadowDepth::IsInBoundingBox()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_vector vCamPos = XMLoadFloat4(pGameInstance->Get_CamPosition());

	Safe_Release(pGameInstance);

	return XMVector3InBounds(XMVectorSubtract(vCamPos, XMLoadFloat3(&m_DepthDesc.vCenter)), XMLoadFloat3(&m_DepthDesc.vExtents));
}

CShadowDepth* CShadowDepth::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CShadowDepth* pInstance = new CShadowDepth(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CShadowDepth");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CShadowDepth::Clone(void* pArg)
{
	CShadowDepth* pInstance = new CShadowDepth(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CShadowDepth");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShadowDepth::Free()
{
	__super::Free();
}
