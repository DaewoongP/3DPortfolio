#include "..\Public\Enemy_Sniper.h"
#include "GameInstance.h"
#include "BloodScreen.h"
#include "BloodDirectional.h"
#include "BloodParticle.h"
#include "MiniPlayer.h"
#include "Sniper.h"

CEnemy_Sniper::CEnemy_Sniper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEnemy(pDevice, pContext)
{
}

CEnemy_Sniper::CEnemy_Sniper(const CEnemy_Sniper& rhs)
	: CEnemy(rhs)
{
}

HRESULT CEnemy_Sniper::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnemy_Sniper::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		m_EnemyDesc = *(static_cast<ENEMYDESC*>(pArg));
		lstrcpy(m_EnemyDesc.pModelPrototypeTag, (static_cast<ENEMYDESC*>(pArg))->pModelPrototypeTag);
	}
	else
	{
		MSG_BOX("Failed Read EnemyDesc");
		return E_FAIL;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(m_EnemyDesc)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(5.f, 2.f, 10.f, 1.f));

	return S_OK;
}

HRESULT CEnemy_Sniper::Initialize_Level(_uint iLevelIndex)
{
	if (FAILED(Add_Parts(iLevelIndex)))
		return E_FAIL;

	m_pTransformCom->Set_Scale(m_EnemyDesc.vScale);
	m_pTransformCom->Rotation(m_EnemyDesc.vRotation);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_EnemyDesc.vPosition));

	CTransform::TRANSFORMDESC TransformDesc = CTransform::TRANSFORMDESC(3.f, XMConvertToRadians(90.f));
	m_pTransformCom->Set_Desc(TransformDesc);

	m_pModelCom->Reset_Animation(1);
	m_pModelCom->Delete_AnimationTranslation(5);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CGameObject* pGameObject = pGameInstance->Find_GameObject(iLevelIndex, TEXT("Layer_Player"), TEXT("GameObject_MiniPlayer"));

	if (nullptr == pGameObject)
	{
		MSG_BOX("Failed Find Player");
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	m_pTargetPlayer = dynamic_cast<CMiniPlayer*>(pGameObject);
	Safe_AddRef(m_pTargetPlayer);

	Safe_Release(pGameInstance);

	m_pTransformCom->LookAt(m_pTargetPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION), true);

	return S_OK;
}

void CEnemy_Sniper::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
}

GAMEEVENT CEnemy_Sniper::Late_Tick(_double dTimeDelta)
{
	AnimationState(dTimeDelta);

	__super::Late_Tick(dTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (nullptr != m_pRendererCom &&
		true == pGameInstance->isIn_WorldFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 3.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_LIGHTDEPTH, this);
	}

	Safe_Release(pGameInstance);

	return PlayEvent(dTimeDelta);
}

HRESULT CEnemy_Sniper::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE);
		m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TextureType_NORMALS);

		m_pShaderCom->Begin(2);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CEnemy_Sniper::Render_LightDepth()
{
	if (FAILED(SetUp_ShadowShaderResources()))
		return E_FAIL;

	// 그림자 매핑 다른 렌더링은 똑같이 처리하면됨.
	// 근데 일단 노말텍스처나 이런 머테리얼 바인딩은 필요없어보임.
	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShadowShaderCom, "g_BoneMatrices", i);

		m_pShadowShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CEnemy_Sniper::Reset()
{
	m_pModelCom->Reset_Animation(1);

	// 상태값 통일하면 애니메이션 초기화도 간단함.
	m_ePreState = STATE_IDLE;
	m_eCurState = STATE_IDLE;

	return S_OK;
}

HRESULT CEnemy_Sniper::Add_Component(ENEMYDESC& EnemyDesc)
{
	/* For.Com_Model */
	if (FAILED(CComposite::Add_Component(LEVEL_STAGE2, TEXT("Prototype_Component_Model_Enemy_Sniper"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		MSG_BOX("Failed CEnemy_Sniper Add_Component : (Com_Model)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		MSG_BOX("Failed CEnemy_Sniper Add_Component : (Com_Shader)");
		return E_FAIL;
	}

	/* For.Com_ShadowShader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimMesh_Shadow"),
		TEXT("Com_ShadowShader"), reinterpret_cast<CComponent**>(&m_pShadowShaderCom))))
	{
		MSG_BOX("Failed CEnemy_Sniper Add_Component : (Com_ShadowShader)");
		return E_FAIL;
	}

	/* For.Com_BloodEffect */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_BloodDirectional"),
		TEXT("Com_BloodEffect"), reinterpret_cast<CComponent**>(&m_pBloodEffect))))
	{
		MSG_BOX("Failed CEnemy_Sniper Add_Component : (Com_BloodEffect)");
		return E_FAIL;
	}

	/* For.Com_BloodParticle */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_BloodParticle"),
		TEXT("Com_BloodParticle"), reinterpret_cast<CComponent**>(&m_pBloodParticle))))
	{
		MSG_BOX("Failed CEnemy_Sniper Add_Component : (Com_BloodParticle)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEnemy_Sniper::Add_Parts(_uint iLevelIndex)
{
	CPart::PARENTMATRIXDESC ParentMatrixDesc;
	ZEROMEM(&ParentMatrixDesc);

	const CBone* pBone = m_pModelCom->Get_Bone(TEXT("Gun_r"));

	ParentMatrixDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	ParentMatrixDesc.OffsetMatrix = pBone->Get_OffsetMatrix();
	ParentMatrixDesc.pCombindTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
	ParentMatrixDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();

	if (FAILED(__super::Add_Part(iLevelIndex, TEXT("Prototype_GameObject_Weapon_Sniper"), TEXT("Layer_EnemyWeapon"),
		TEXT("Part_Sniper"), reinterpret_cast<CGameObject**>(&m_pSniper), &ParentMatrixDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnemy_Sniper::SetUp_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", pGameInstance->Get_CamFar(), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPos", pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;
	_float fRimWidth = 0.5f;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fRimWidth", &fRimWidth, sizeof(_float))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CEnemy_Sniper::SetUp_ShadowShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShadowShaderCom->Bind_RawValue("g_fFar", pGameInstance->Get_LightFar(), sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShadowShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShadowShaderCom->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_LightDepthFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShadowShaderCom->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_LightDepthFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

void CEnemy_Sniper::AnimationState(_double dTimeDelta)
{
	m_eCurrentAnimationFlag = m_pModelCom->Get_AnimationState();

	if (ANIM_FINISHED == m_eCurrentAnimationFlag &&
		m_ePreState == m_eCurState)
	{
		m_eCurState = STATE_IDLE;
	}

	Motion_Change(m_eCurrentAnimationFlag);

	m_pModelCom->Play_Animation(dTimeDelta);
}

void CEnemy_Sniper::Motion_Change(ANIMATIONFLAG eAnimationFlag)
{
	if (false == (ANIM_PLAYING == eAnimationFlag || ANIM_FINISHED == eAnimationFlag))
		return;

	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case STATE_IDLE:
			m_pModelCom->Set_AnimIndex(1);
			break;
		case STATE_ATTACK:
			m_pModelCom->Set_AnimIndex(0, false);
			break;
		case STATE_DEAD:
			m_pModelCom->Set_AnimIndex(2 + rand() % 3, false);
			break;
		}

		m_ePreState = m_eCurState;
	}
}

void CEnemy_Sniper::Attack(_double dArriveTime)
{
	_vector vTargetPos = m_pTargetPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float fLength = XMVectorGetX(XMVector3Length(vTargetPos - vPos));
	// Length per Time
	_double dSpeed = fLength / dArriveTime;

	m_pSniper->Attack(m_pTransformCom->Get_State(CTransform::STATE_POSITION),
		m_pTargetPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION), dSpeed);
	m_eCurState = STATE_ATTACK;
}

void CEnemy_Sniper::Delete_Bullet()
{
	m_pSniper->Delete_Bullet();
}

GAMEEVENT CEnemy_Sniper::PlayEvent(_double dTimeDelta)
{
	if (GAME_OBJECT_DEAD == m_eGameEvent)
	{
		if (false == m_isDead)
		{
			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			Safe_AddRef(pGameInstance);
			pGameInstance->Play_Sound(TEXT("Enemy_Death%d.ogg"), 9, CSound_Manager::SOUND_PISTOL, 0.3f, true);
			Safe_Release(pGameInstance);
		}

		m_eCurState = STATE_DEAD;
		m_isDead = true;

		if (ANIM_FINISHED == m_eCurrentAnimationFlag)
			return GAME_OBJECT_DEAD;
	}

	return GAME_NOEVENT;
}

CEnemy_Sniper* CEnemy_Sniper::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEnemy_Sniper* pInstance = new CEnemy_Sniper(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CEnemy_Sniper");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEnemy_Sniper::Clone(void* pArg)
{
	CEnemy_Sniper* pInstance = new CEnemy_Sniper(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CEnemy_Sniper");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEnemy_Sniper::Free()
{
	__super::Free();

	Safe_Release(m_pTargetPlayer);

	Safe_Release(m_pBloodEffect);
	Safe_Release(m_pBloodParticle);

	Safe_Release(m_pSniper);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pShadowShaderCom);
}
