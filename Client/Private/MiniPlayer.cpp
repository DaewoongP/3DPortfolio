#include "..\Public\MiniPlayer.h"
#include "GameInstance.h"
#include "Katana.h"
#include "BlockEffect.h"
#include "MiniGame_Manager.h"

CMiniPlayer::CMiniPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CMiniPlayer::CMiniPlayer(const CMiniPlayer& rhs)
	: CGameObject(rhs)

#ifdef _DEBUG
	, m_isMouseFixed(rhs.m_isMouseFixed)
#endif // _DEBUG
{
}

HRESULT CMiniPlayer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMiniPlayer::Initialize(void* pArg)
{
	CTransform::TRANSFORMDESC TransformDesc = CTransform::TRANSFORMDESC(0.f, XMConvertToRadians(3.f));
	if (FAILED(__super::Initialize(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pModelCom->Set_AnimIndex(95);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 8.f, 0.f, 1.f));

	m_isMouseFixed = true;

	return S_OK;
}

HRESULT CMiniPlayer::Initialize_Level(_uint iLevelIndex)
{
	if (FAILED(Add_Parts(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

void CMiniPlayer::Tick(_double dTimeDelta)
{
	Key_Input(dTimeDelta);

	Fix_Mouse();

	AnimationState(dTimeDelta);

	if (STATE_BLOCK == m_eCurState)
		m_pKatana->Add_TrailRender();

	__super::Tick(dTimeDelta);

	CameraOffset(dTimeDelta);

	m_pCameraCom->Tick_Camera(dTimeDelta);
}

GAMEEVENT CMiniPlayer::Late_Tick(_double dTimeDelta)
{
	if (GAME_STAGE_RESET != m_eGameEvent &&
		nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	}

	__super::Late_Tick(dTimeDelta);

	return m_eGameEvent;
}

HRESULT CMiniPlayer::Render()
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

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CMiniPlayer::Reset()
{
	m_pModelCom->Set_AnimIndex(95);

	m_pRendererCom->Set_GrayScale(false);
	m_pRendererCom->Set_RedScale(false);

	m_eCurState = STATE_IDLE;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	pGameInstance->Set_SlowedTime(TEXT("MainTimer"), 1.0);

	Safe_Release(pGameInstance);

	if (FAILED(__super::Reset()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMiniPlayer::Add_Component()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSG_BOX("Failed CMiniPlayer Add_Component : (Com_Renderer)");
		return E_FAIL;
	}

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Player"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSG_BOX("Failed CMiniPlayer Add_Component : (Com_Model)");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSG_BOX("Failed CMiniPlayer Add_Component : (Com_Shader)");
		return E_FAIL;
	}

	CCamera::CAMERADESC CameraDesc;
	CameraDesc.vEye = _float4(5.f, 5.f, 5.f, 1.f);
	CameraDesc.vAt = _float4(10.f, 0.f, 10.f, 1.f);
	CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);

	CameraDesc.fFovy = XMConvertToRadians(90.f);
	CameraDesc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 1000.f;
	CameraDesc.TransformDesc.dSpeedPerSec = m_pTransformCom->Get_Desc().dSpeedPerSec;
	CameraDesc.TransformDesc.dRotationPerSec = m_pTransformCom->Get_Desc().dRotationPerSec;

	/* For.Com_Camera */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Camera"),
		TEXT("Com_Camera"), reinterpret_cast<CComponent**>(&m_pCameraCom), &CameraDesc)))
	{
		MSG_BOX("Failed CPlayer Add_Component : (Com_Camera)");
		return E_FAIL;
	}

	/* For.Com_BlockEffect */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_BlockEffect"),
		TEXT("Com_BlockEffect"), reinterpret_cast<CComponent**>(&m_pBlockEffect))))
	{
		MSG_BOX("Failed CPlayer Add_Component : (Com_BlockEffect)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMiniPlayer::Add_Parts(_uint iLevelIndex)
{
	CPart::PARENTMATRIXDESC ParentMatrixDesc;
	ZEROMEM(&ParentMatrixDesc);

	const CBone* pBone = m_pModelCom->Get_Bone(TEXT("Weapon_r"));

	ParentMatrixDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	ParentMatrixDesc.OffsetMatrix = pBone->Get_OffsetMatrix();
	ParentMatrixDesc.pCombindTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
	ParentMatrixDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();

	if (FAILED(__super::Add_Part(iLevelIndex, TEXT("Prototype_GameObject_Katana"), TEXT("Layer_PlayerWeapon"),
		TEXT("Part_Katana"), reinterpret_cast<CGameObject**>(&m_pKatana), &ParentMatrixDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMiniPlayer::SetUp_ShaderResources()
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

	Safe_Release(pGameInstance);

	return S_OK;
}

void CMiniPlayer::Key_Input(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_DOWN))
	{
		if (STATE_IDLE == m_eCurState)
		{
			CMiniGame_Manager* pMiniGame_Manager = CMiniGame_Manager::GetInstance();
			Safe_AddRef(pMiniGame_Manager);

			pMiniGame_Manager->Set_Blocked(true);

			Safe_Release(pMiniGame_Manager);
		}
	}

	_long		dwMouseMove = 0;

#ifdef _DEBUG
	if (false == m_isMouseFixed)
	{
		Safe_Release(pGameInstance);
		return;
	}
#endif // _DEBUG

	Safe_Release(pGameInstance);
}

void CMiniPlayer::Fix_Mouse()
{
	if (GAME_OBJECT_DEAD == m_eGameEvent)
		return;
#ifdef _DEBUG
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	// 키보드 ` 키
	if (pGameInstance->Get_DIKeyState(DIK_GRAVE, CInput_Device::KEY_DOWN))
	{
		if (m_isMouseFixed)
			m_isMouseFixed = false;
		else
			m_isMouseFixed = true;
	}

	Safe_Release(pGameInstance);

	if (false == m_isMouseFixed)
		return;
#endif // _DEBUG
	POINT	ptMouse{ g_iWinSizeX >> 1, g_iWinSizeY >> 1 };

	ClientToScreen(g_hWnd, &ptMouse);
	SetCursorPos(ptMouse.x, ptMouse.y);
}

void CMiniPlayer::AnimationState(_double dTimeDelta)
{
	m_eCurrentAnimationFlag = m_pModelCom->Get_AnimationState();

	if (ANIM_FINISHED == m_eCurrentAnimationFlag &&
		m_ePreState == m_eCurState)
	{
		m_eCurState = STATE_IDLE;
	}

	CMiniGame_Manager* pMiniGame_Manager = CMiniGame_Manager::GetInstance();
	Safe_AddRef(pMiniGame_Manager);


	switch (pMiniGame_Manager->Get_State())
	{
	case CMiniGame_Manager::STATE_PERFECT:
		m_eCurState = STATE_BLOCK;
		break;
	case CMiniGame_Manager::STATE_GREAT:
		m_eCurState = STATE_BLOCK;
		break;
	case CMiniGame_Manager::STATE_FAIL:
		m_eCurState = STATE_DEAD;
		break;
	case CMiniGame_Manager::STATE_WAIT:
		m_eCurState = STATE_IDLE;
		break;
	default:
		// pass
		break;
	}

	Safe_Release(pMiniGame_Manager);

	Motion_Change(m_eCurrentAnimationFlag);

	m_pModelCom->Play_Animation(dTimeDelta);
}

void CMiniPlayer::Motion_Change(ANIMATIONFLAG eAnimationFlag)
{
	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case STATE_IDLE:
			m_pModelCom->Set_AnimIndex(95);
			break;
		case STATE_BLOCK:
			m_pModelCom->Set_AnimIndex(78 + rand() % 3, false);
			_float4 vWeaponPos;
			memcpy(&vWeaponPos, m_pKatana->Get_CombinedWorldMatrix().m[3], sizeof(_float4));
			m_pBlockEffect->Render_Effect(XMLoadFloat4(&vWeaponPos) + m_pTransformCom->Get_State(CTransform::STATE_LOOK));
			break;
		case STATE_DEAD:
			m_pModelCom->Set_AnimIndex(169 + rand() % 10, false);
			break;
		default:
			MSG_BOX("Mini Player State Error");
			break;
		}

		m_ePreState = m_eCurState;
	}
}

void CMiniPlayer::CameraOffset(_double dTimeDelta)
{
	_float4 vEye, vAt;

	// 위치값은 고정
	XMStoreFloat4(&vEye, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	XMStoreFloat4(&vAt, XMLoadFloat4(&vEye) + m_pTransformCom->Get_State(CTransform::STATE_LOOK));

	m_pCameraCom->Set_LookAtLH(vEye, vAt);
}

CMiniPlayer* CMiniPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMiniPlayer* pInstance = new CMiniPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CMiniPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMiniPlayer::Clone(void* pArg)
{
	CMiniPlayer* pInstance = new CMiniPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CMiniPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMiniPlayer::Free()
{
	__super::Free();

	Safe_Release(m_pKatana);

	Safe_Release(m_pBlockEffect);

	Safe_Release(m_pModelCom);
	Safe_Release(m_pCameraCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
