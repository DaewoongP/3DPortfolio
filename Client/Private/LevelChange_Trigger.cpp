#include "..\Public\LevelChange_Trigger.h"
#include "GameInstance.h"
#include "Portal.h"
#include "Level_Stage1.h"

CLevelChange_Trigger::CLevelChange_Trigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTrigger(pDevice, pContext)
{
}

CLevelChange_Trigger::CLevelChange_Trigger(const CLevelChange_Trigger& rhs)
	: CTrigger(rhs)
{
}

HRESULT CLevelChange_Trigger::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevelChange_Trigger::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	// 위치세팅 및 콜라이더 크기 세팅.
	CBounding_AABB::BOUNDINGAABBDESC AABBDesc;
	AABBDesc.vPosition = _float3(0.f, 0.f, 0.f);
	AABBDesc.vExtents = _float3(10.f, 10.f, 10.f);
	m_pColliderCom->Set_BoundingDesc(&AABBDesc);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(529.5f, 9.f, 102.5f, 1.f));
	
#ifdef _DEBUG
	m_vColliderColor = _float4(1.f, 1.f, 1.f, 1.f);
	m_pColliderCom->Set_Color(XMLoadFloat4(&m_vColliderColor));
#endif // _DEBUG

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pPortal->Get_Transform()->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return S_OK;
}

void CLevelChange_Trigger::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
}

GAMEEVENT CLevelChange_Trigger::Late_Tick(_double dTimeDelta)
{
	return __super::Late_Tick(dTimeDelta);
}

void CLevelChange_Trigger::OnCollisionEnter(COLLISIONDESC CollisionDesc)
{

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CLevel* pLevel = pGameInstance->Get_CurrentLevel();
	dynamic_cast<CLevel_Stage1*>(pLevel)->Set_NextLevel();

	Safe_Release(pGameInstance);
}

void CLevelChange_Trigger::OnCollisionStay(COLLISIONDESC CollisionDesc)
{
}

void CLevelChange_Trigger::OnCollisionExit(COLLISIONDESC CollisionDesc)
{
}

HRESULT CLevelChange_Trigger::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevelChange_Trigger::Reset()
{
	if (FAILED(__super::Reset()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevelChange_Trigger::Add_Components()
{
	/* For.Com_Portal */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_HOS_Portal"),
		TEXT("Com_Portal"), (CComponent**)&m_pPortal)))
		return E_FAIL;

	return S_OK;
}

CLevelChange_Trigger* CLevelChange_Trigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevelChange_Trigger* pInstance = new CLevelChange_Trigger(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CLevelChange_Trigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLevelChange_Trigger::Clone(void* pArg)
{
	CLevelChange_Trigger* pInstance = new CLevelChange_Trigger(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CLevelChange_Trigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevelChange_Trigger::Free()
{
	__super::Free();

	Safe_Release(m_pPortal);
}
