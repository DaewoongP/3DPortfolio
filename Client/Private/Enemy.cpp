#include "..\Public\Enemy.h"
#include "GameInstance.h"

CEnemy::CEnemy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEnemy::CEnemy(const CEnemy& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEnemy::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnemy::Initialize(void* pArg)
{
	CTransform::TRANSFORMDESC TransformDesc = CTransform::TRANSFORMDESC(0.f, XMConvertToRadians(3.f));

	if (FAILED(__super::Initialize(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnemy::Initialize_Level(_uint iLevelIndex)
{
	if (FAILED(Add_Component_Level(iLevelIndex)))
		return E_FAIL;

	// 네비게이션 초기위치 찾기.
	m_pNavigationCom->Find_MyCell(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return S_OK;
}

void CEnemy::Tick(_double dTimeDelta)
{	
	__super::Tick(dTimeDelta);
}

GAMEEVENT CEnemy::Late_Tick(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (nullptr != m_pRendererCom &&
		true == pGameInstance->isIn_WorldFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 4.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
#ifdef _DEBUG
		m_pRendererCom->Add_DebugGroup(m_pNavigationCom);
#endif // _DEBUG
	}
	
	__super::Late_Tick(dTimeDelta);

	Safe_Release(pGameInstance);

	return GAME_NOEVENT;
}

void CEnemy::OnCollisionEnter(COLLISIONDESC CollisionDesc)
{
}

void CEnemy::OnCollisionStay(COLLISIONDESC CollisionDesc)
{
	if (COLLISIONDESC::COLTYPE_ENEMY == CollisionDesc.ColType)
	{
		_vector vOtherPos = CollisionDesc.pOtherTransform->Get_State(CTransform::STATE_POSITION);
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		m_pTransformCom->Move_Direction(XMVector3Normalize(vPos - vOtherPos), g_TimeDelta);
	}
}

void CEnemy::OnCollisionExit(COLLISIONDESC CollisionDesc)
{
}

HRESULT CEnemy::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnemy::Reset()
{
	m_pTransformCom->Set_Scale(m_EnemyDesc.vScale);
	m_pTransformCom->Rotation(m_EnemyDesc.vRotation);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_EnemyDesc.vPosition));

	if (FAILED(__super::Reset()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnemy::Add_Component()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		MSG_BOX("Failed CEnemy Add_Component : (Com_Renderer)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEnemy::Add_Component_Level(_uint iLevelIndex)
{
	CNavigation::NAVIGATIONDESC NavigationDesc;
	NavigationDesc.iCurrentIndex = 0;

	/* For.Com_Navigation */
	if (FAILED(__super::Add_Component(iLevelIndex, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NavigationDesc)))
	{
		MSG_BOX("Failed CEnemy Add_Component : (Com_Navigation)");
		return E_FAIL;
	}

	return S_OK;
}

void CEnemy::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pRendererCom);
}
