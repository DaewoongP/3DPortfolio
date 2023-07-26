#include "..\Public\MiniEnemy.h"
#include "GameInstance.h"
#include "Layer.h"

CMiniEnemy::CMiniEnemy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CMiniEnemy::CMiniEnemy(const CMiniEnemy& rhs)
	: CUI(rhs)
{
}

HRESULT CMiniEnemy::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMiniEnemy::Initialize(void* pArg)
{
	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fSizeX = 32.f;
	m_fSizeY = 32.f;

	m_vUIPos = _float2(g_iWinSizeX * 0.92f, g_iWinSizeY * 0.13f);
	m_vOffSet = _float2(1.f, 1.f);

	m_fMaxEnemyDistance = 70.f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMiniEnemy::Initialize_Level(_uint iLevelIndex)
{
	if (FAILED(__super::Initialize_Level(iLevelIndex)))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_pEnemyLayer = pGameInstance->Make_Layer(iLevelIndex, TEXT("Layer_Enemy"));
	if (LEVEL_BOSS == iLevelIndex)
		m_pBossLayer = pGameInstance->Make_Layer(iLevelIndex, TEXT("Layer_Boss"));

	Safe_Release(pGameInstance);

	return S_OK;
}

void CMiniEnemy::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	if (nullptr != m_pEnemyLayer)
	{
		for (auto& pair : m_pEnemyLayer->Get_AllGameObject())
		{
			CTransform* pTransform = pair.second->Get_Transform();
			m_EnemyTransforms.push_back(pTransform);
			Safe_AddRef(pTransform);
		}
	}
	
	if (nullptr != m_pBossLayer)
	{
		for (auto& pair : m_pBossLayer->Get_AllGameObject())
		{
			CTransform* pTransform = pair.second->Get_Transform();
			m_EnemyTransforms.push_back(pTransform);
			Safe_AddRef(pTransform);
		}
	}
}

GAMEEVENT CMiniEnemy::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_vector vCamPos = XMLoadFloat4(pGameInstance->Get_CamPosition());
	_matrix ViewMatrix = pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);

	for (auto& pTransform : m_EnemyTransforms)
	{
		_vector vEnemyPos = pTransform->Get_State(CTransform::STATE_POSITION);
		// continue 전 삭제.
		Safe_Release(pTransform);
		// 거리 50이 넘으면 패스 및 연산 감소를 위해 아직 뷰스페이스로 넘기지 않음.
		if (m_fMaxEnemyDistance < XMVectorGetX(XMVector4Length(vEnemyPos - vCamPos)))
			continue;
		// 뷰스페이스 변환 (카메라위치 원점으로 처리하면됨.)
		_vector vEnemyViewPosition = XMVector3TransformCoord(vEnemyPos, ViewMatrix);

		// 뷰스페이스의 양의좌표 = UI상에서의 음의 좌표 따라서 Y에 -1
		m_UIPositions.push_back(_float2(
			m_vUIPos.x + XMVectorGetX(vEnemyViewPosition) * m_vOffSet.x,
			m_vUIPos.y + XMVectorGetZ(vEnemyViewPosition) * m_vOffSet.y * -1.f));
	}

	m_EnemyTransforms.clear();

	Safe_Release(pGameInstance);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return GAME_NOEVENT;
}

HRESULT CMiniEnemy::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	for (auto& vPosition : m_UIPositions)
	{
		m_fX = vPosition.x;
		m_fY = vPosition.y;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION,
			XMVectorSet(
				m_fX - g_iWinSizeX * 0.5f,
				-m_fY + g_iWinSizeY * 0.5f,
				0.f,
				1.f));

		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
			return E_FAIL;

		m_pShaderCom->Begin(1);

		m_pVIBufferCom->Render();
	}

	m_UIPositions.clear();

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMiniEnemy::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MiniEnemy"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX("Failed BackGround Add_Component : (Com_Texture)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMiniEnemy::SetUp_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	return S_OK;
}

CMiniEnemy* CMiniEnemy::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMiniEnemy* pInstance = new CMiniEnemy(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CMiniEnemy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMiniEnemy::Clone(void* pArg)
{
	CMiniEnemy* pInstance = new CMiniEnemy(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CMiniEnemy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMiniEnemy::Free()
{
	__super::Free();

	for (auto& pTransform : m_EnemyTransforms)
		Safe_Release(pTransform);

	m_EnemyTransforms.clear();
}