#include "..\Public\AnimationNotify.h"
#include "Model.h"
#include "AnimModel.h"
#include "NonAnimModel.h"

CAnimationNotify::CAnimationNotify()
{
	ZEROMEM(&m_vEye);
	m_vEye.w = 1.f;
	ZEROMEM(&m_vAt);
}

void CAnimationNotify::Set_CurrentAnimationObject(CAnimModel* pAnimDummy, CModel* pModel)
{
	if (nullptr == pModel ||
		nullptr == pAnimDummy)
		return;
	m_pCurrentAnimDummy = pAnimDummy;
	m_pCurrentModelCom = pModel;
}

void CAnimationNotify::Set_CurrentIndex(_uint iAnimaitonIndex, _uint iFrameIndex)
{
	m_iAnimationIndex = iAnimaitonIndex;
	m_iFrameIndex = iFrameIndex;
}

HRESULT CAnimationNotify::Initialize(void* pArg)
{
	m_pToolCamera = static_cast<CNonAnimModel*>(m_pGameInstance->Find_GameObject(LEVEL_TOOL, TEXT("Layer_Tool"), TEXT("GameObject_ToolCamera")));
	if (nullptr == m_pToolCamera)
		return E_FAIL;
	Safe_AddRef(m_pToolCamera);
	m_pToolCameraTrans = m_pToolCamera->Get_TransformCom();
	Safe_AddRef(m_pToolCameraTrans);
	m_pToolCameraTrans->Set_Scale(_float3(0.5f, 0.5f, 0.5f));
	m_pToolCameraTrans->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, -50.f, 1.f));
	
	return S_OK;
}

void CAnimationNotify::Tick(_double dTimeDelta)
{
	ImGui::Begin("Animation Notify");
	// 사운드, 파티클, 이펙트, 카메라 등등의 처리
	if (nullptr != m_pCurrentModelCom && ImGui::BeginTabBar("Notify"))
	{
		if (ImGui::BeginTabItem("Camera"))
		{
			NotifyCamera();

			RenderCamera();

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}

HRESULT CAnimationNotify::NotifyCamera()
{
	ImGui::Checkbox("Camera On", &m_bCamera);
	if (true == m_bCamera)
	{
		// 현재 플레이어에서의 포지션 오프셋, -> 이걸 변경할 수 있게 만들고
		if (ImGui::InputFloat3("Camera Offset Eye", reinterpret_cast<_float*>(&m_vEye)))
		{
			m_pToolCameraTrans->Set_State(CTransform::STATE_POSITION, 
				m_pCurrentAnimDummy->Get_TransformCom()->Get_State(CTransform::STATE_POSITION) +
				XMLoadFloat4(&m_vEye));
		}

		ImGui::InputFloat3("Camera Offset LookAt", reinterpret_cast<_float*>(&m_vAt));
		// 월드상에서의 위치값. -> 이건그냥 보기위한 용도로 처리하자.
	}

	return S_OK;
}

HRESULT CAnimationNotify::RenderCamera()
{
	

	return S_OK;
}

CAnimationNotify* CAnimationNotify::Create(void* pArg)
{
	CAnimationNotify* pInstance = New CAnimationNotify();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CAnimationNotify");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimationNotify::Free()
{
	__super::Free();

	Safe_Release(m_pToolCameraTrans);
	Safe_Release(m_pToolCamera);
}