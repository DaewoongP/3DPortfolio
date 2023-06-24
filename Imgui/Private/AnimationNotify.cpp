#include "..\Public\AnimationNotify.h"
#include "Model.h"
#include "AnimModel.h"
#include "NonAnimModel.h"
#include "Camera_Free.h"

CAnimationNotify::CAnimationNotify()
{
	ZEROMEM(&m_vEye);
	ZEROMEM(&m_vAt);
}

void CAnimationNotify::Set_CurrentAnimationObject(CAnimModel* pAnimDummy, CModel* pModel)
{
	if (nullptr == pModel ||
		nullptr == pAnimDummy)
		return;

	m_pCurrentAnimDummy = pAnimDummy;
	m_pCurrentModelCom = pModel;

	m_FrameNotify.clear();
	m_FrameNotify = m_pCurrentModelCom->Get_CurrentAnimationNotify();
}

void CAnimationNotify::Set_CurrentAnimationValue(_uint iAnimaitonIndex, _uint iAnimationFrames)
{
	m_iAnimationIndex = iAnimaitonIndex;
	m_iFrames = iAnimationFrames;
}

void CAnimationNotify::Set_CurrentFrameIndex(_uint iCurrentFrameIndex)
{
	m_iCurrentFrameIndex = iCurrentFrameIndex;
}

HRESULT CAnimationNotify::Initialize(void* pArg)
{
	m_FrameSpeedFunc = Funcs::FrameSpeedSaw;

	m_pToolCamera = static_cast<CNonAnimModel*>(m_pGameInstance->Find_GameObject(LEVEL_TOOL, TEXT("Layer_Tool"), TEXT("GameObject_ToolCamera")));
	
	if (nullptr == m_pToolCamera)
		return E_FAIL;

	Safe_AddRef(m_pToolCamera);

	m_pToolCameraTrans = m_pToolCamera->Get_TransformCom();
	Safe_AddRef(m_pToolCameraTrans);

	m_pToolCameraTrans->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, -50.f, 1.f));
	
	return S_OK;
}

void CAnimationNotify::Tick(_double dTimeDelta)
{
	// 사운드, 파티클, 이펙트, 카메라 등등의 처리
	if (nullptr != m_pCurrentModelCom && ImGui::BeginTabBar("Notify"))
	{
		if (ImGui::BeginTabItem("Speed"))
		{
			NotifySpeed();

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Camera"))
		{
			CameraSetLerp();

			NotifyCamera();

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}

HRESULT CAnimationNotify::NotifyCamera()
{
	InputFrameIndex();

	if (ImGui::Checkbox("Camera On", &m_bCamera))
	{
		if (true == m_bCamera)
		{
			XMStoreFloat4x4(&m_DummyMatrix, m_pCurrentAnimDummy->Get_TransformCom()->Get_WorldMatrix());

			m_pCurrentAnimDummy->Get_TransformCom()->Set_WorldMatrix(XMMatrixIdentity());

			// 현재 카메라위치 원점쪽으로 이동
			CCamera_Free* pCam = dynamic_cast<CCamera_Free*>(m_pGameInstance->Find_GameObject(LEVEL_TOOL, TEXT("Layer_Tool"), TEXT("GameObject_Camera_Free")));
			
			pCam->Set_CameraView(_float4(0.f, 5.f, 3.f, 1.f), _float4(0.f, 2.f, 0.f, 1.f), _float4(0.f, 1.f, 0.f, 0.f));
		}
		else
		{
			m_pCurrentAnimDummy->Get_TransformCom()->Set_WorldMatrix(XMLoadFloat4x4(&m_DummyMatrix));
		}
	}

	if (true == m_bCamera)
	{
		// 현재 플레이어에서의 포지션 오프셋, -> 이걸 변경할 수 있게 만들고
		if (ImGui::InputFloat3("Offset Eye", reinterpret_cast<_float*>(&m_vEye)))
		{
			if (false == XMVector3Equal(XMLoadFloat4(&m_vEye), XMLoadFloat4(&m_vAt)))
			{
				if (0 == m_iCurrentFrameIndex)
					m_is0FrameSet = true;

				_matrix CamMatrix = XMMatrixInverse(nullptr, XMMatrixLookAtLH(XMLoadFloat4(&m_vEye), XMLoadFloat4(&m_vAt), XMVectorSet(0.f, 1.f, 0.f, 0.f)));

				m_pToolCameraTrans->Set_WorldMatrix(CamMatrix);
				
				m_FrameNotify[m_iCurrentFrameIndex].vEye = m_vEye;
				m_FrameNotify[m_iCurrentFrameIndex].vAt = m_vAt;

				m_pCurrentModelCom->Set_AnimationFrameCamera(m_iCurrentFrameIndex, m_FrameNotify[m_iCurrentFrameIndex].vEye, m_FrameNotify[m_iCurrentFrameIndex].vAt);
			}
		}

		if (ImGui::InputFloat3("Offset LookAt", reinterpret_cast<_float*>(&m_vAt)))
		{
			if (false == XMVector3Equal(XMLoadFloat4(&m_vEye), XMLoadFloat4(&m_vAt)))
			{
				if (0 == m_iCurrentFrameIndex)
					m_is0FrameSet = true;

				_matrix CamMatrix = XMMatrixInverse(nullptr, XMMatrixLookAtLH(XMLoadFloat4(&m_vEye), XMLoadFloat4(&m_vAt), XMVectorSet(0.f, 1.f, 0.f, 0.f)));

				m_pToolCameraTrans->Set_WorldMatrix(CamMatrix);
				
				m_FrameNotify[m_iCurrentFrameIndex].vEye = m_vEye;
				m_FrameNotify[m_iCurrentFrameIndex].vAt = m_vAt;

				m_pCurrentModelCom->Set_AnimationFrameCamera(m_iCurrentFrameIndex, m_FrameNotify[m_iCurrentFrameIndex].vEye, m_FrameNotify[m_iCurrentFrameIndex].vAt);
			}
		}

	}

	if (ImGui::Checkbox("Camera Test", &m_bTestCamera))
	{
		if (m_bTestCamera)
		{
			if (false == m_is0FrameSet)
			{
				MSG_BOX("Camera Notify 0 Frame Not Set");
				m_bTestCamera = false;
			}
		}
	}

	if (m_bTestCamera)
	{
		TestCamera();
	}

	return S_OK;
}

HRESULT CAnimationNotify::CameraSetLerp()
{
	if (false == ImGui::Button("Notify Camera Lerp"))
		return S_OK;

	if (false == m_is0FrameSet)
	{
		MSG_BOX("Camera Notify 0 Frame Not Set");
		return S_OK;
	}

	_float4 vEye;
	ZEROMEM(&vEye);
	_float4 vAt;
	ZEROMEM(&vAt);

	// SRT중 가장 많은 프레임을 가진 값을 순회하며 각 프레임에서의 상태값을 저장.
	// 만약 해당 프레임에서 값이 없을 경우 마지막으로 처리한 값으로 저장.
	for (_uint i = 0; i < m_iFrames; ++i)
	{
		NOTIFY	Notify;

		
	}

	return S_OK;
}

HRESULT CAnimationNotify::TestCamera()
{
	// 애니메이션에 스피드 처럼 세팅을 하면
	// 그 키프레임에 맞는 노티파이를 가져와서 freecam을 움직여주는게 처리가 편할듯.

	if (false == XMVector3Equal(XMLoadFloat4(&m_FrameNotify[m_iCurrentFrameIndex].vEye),
		XMLoadFloat4(&m_FrameNotify[m_iCurrentFrameIndex].vAt)))
	{
		CCamera_Free* pCam = dynamic_cast<CCamera_Free*>(m_pGameInstance->Find_GameObject(LEVEL_TOOL, TEXT("Layer_Tool"), TEXT("GameObject_Camera_Free")));
		
		pCam->Set_CameraView(m_FrameNotify[m_iCurrentFrameIndex].vEye,
			m_FrameNotify[m_iCurrentFrameIndex].vAt,
			_float4(0.f, 1.f, 0.f, 0.f));
	}

	return S_OK;
}

HRESULT CAnimationNotify::NotifySpeed()
{
	InputFrameIndex();

	// 현재 프레임의 스피드 처리
	ImGui::SetNextItemWidth(100.f);
	SameLine();
	if (ImGui::InputFloat("Frame Speed", &m_FrameNotify[m_iCurrentFrameIndex].fSpeed, 1.f, 0.f, "%.2f"))
	{
		if (0 > m_FrameNotify[m_iCurrentFrameIndex].fSpeed)
			m_FrameNotify[m_iCurrentFrameIndex].fSpeed = 0.1f;
		m_pCurrentModelCom->Set_AnimationFrameSpeed(m_iCurrentFrameIndex, m_FrameNotify[m_iCurrentFrameIndex].fSpeed);
	}


	// 애니메이션 전체적인 스피드값을 그래프로 보여줌.
	vector<_float>		Speeds;
	for (_uint i = 0; i < m_FrameNotify.size(); ++i)
	{
		Speeds.push_back(m_FrameNotify[i].fSpeed);
	}
		
	ImGui::PlotHistogram("Animation Speeds", m_FrameSpeedFunc, &Speeds, m_iFrames, 0, nullptr, 0.f, 10.f, ImVec2(0, 100));

	return S_OK;
}

HRESULT CAnimationNotify::InputFrameIndex()
{
	ImGui::SetNextItemWidth(100.f);
	// 현재 프레임 인덱스 처리
	if (ImGui::InputInt("Frame Index", (_int*)&m_iCurrentFrameIndex))
	{
		if (0 > m_iCurrentFrameIndex)
			m_iCurrentFrameIndex = 0;
		else if (m_iFrames - 1 <= m_iCurrentFrameIndex)
			m_iCurrentFrameIndex = m_iFrames - 1;
		m_pCurrentModelCom->Set_CurrentKeyFrameIndex(m_iCurrentFrameIndex);
	}

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