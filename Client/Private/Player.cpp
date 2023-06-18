#include "..\Public\Player.h"
#include "GameInstance.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(pArg), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(5.f, 0.f, 5.f, 1.f));
	m_pModelCom->Set_AnimIndex(0);
	return S_OK;
}

void CPlayer::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
	m_pModelCom->Play_Animation(dTimeDelta);
	FirstPersonView();
}

void CPlayer::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CPlayer::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE);
		// m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TextureType_NORMALS);

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CPlayer::Add_Component()
{
	/* For.Com_Model */
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fiona"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom)), E_FAIL);

	// Get Model's Bone Index
	FAILED_CHECK_RETURN(Find_BoneIndices(), E_FAIL);

	/* For.Com_Shader */
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)), E_FAIL);

	/* For.Com_Renderer */
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom)), E_FAIL);


	CTransform::TRANSFORMDESC TransformDesc;
	TransformDesc.dSpeedPerSec = 10.f;
	TransformDesc.dRotationPerSec = 10.f;
	/* For.Com_Transform */
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc), E_FAIL);

	CCamera::CAMERADESC CameraDesc;
	CameraDesc.vEye = _float4(-2.f, 3.f, -2.f, 1.f);
	CameraDesc.vAt = _float4(5.f, 0.f, 5.f, 1.f);
	CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);

	CameraDesc.fFovy = XMConvertToRadians(90.f);
	CameraDesc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 1000.f;
	CameraDesc.TransformDesc.dSpeedPerSec = 5.f;
	CameraDesc.TransformDesc.dRotationPerSec = 3.f;

	/* For.Com_Camera */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Camera"),
		TEXT("Com_Camera"), reinterpret_cast<CComponent**>(&m_pPlayerFirstPersonViewCameraCom), &CameraDesc)))
		return E_FAIL;

	FirstPersonView();
	
	return S_OK;
}

HRESULT CPlayer::SetUp_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CPlayer::Find_BoneIndices()
{
	if (FAILED(m_pModelCom->Find_BoneIndex(TEXT("head_end"), &m_iHeadChannelIndex)))
		return E_FAIL;

	return S_OK;
}

void CPlayer::FirstPersonView()
{
	_float4x4 HeadBoneMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix(m_iHeadChannelIndex);
	_float4x4 WorldMatrix = *m_pTransformCom->Get_WorldFloat4x4();

	_matrix	PivotMatrix = XMMatrixTranslation(-HeadBoneMatrix._41, HeadBoneMatrix._42, -HeadBoneMatrix._43 + 0.5f);
	_matrix CamMatrix = PivotMatrix * XMLoadFloat4x4(&WorldMatrix);
	m_pPlayerFirstPersonViewCameraCom->Set_CameraWorldMatrix(CamMatrix);
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CPlayer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();
	Safe_Release(m_pPlayerFirstPersonViewCameraCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
