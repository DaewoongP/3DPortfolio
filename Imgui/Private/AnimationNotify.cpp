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
	m_SetFrames.clear();
	m_FrameNotify = m_pCurrentModelCom->Get_CurrentAnimationNotify();
	m_SetFrames.resize(m_FrameNotify.size());

	for (auto& Notify : m_FrameNotify)
	{
		Notify.vEye.w = 1.f;
		Notify.vAt.w = 1.f;
	}
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
	// ����, ��ƼŬ, ����Ʈ, ī�޶� ����� ó��
	if (nullptr != m_pCurrentModelCom && ImGui::BeginTabBar("Notify"))
	{
		if (ImGui::BeginTabItem("Speed"))
		{
			NotifySpeed();

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Camera"))
		{
			NotifyCamera();

			CameraSetLerp();

			LerpReset();

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
	if (m_iFrames >= 30)
		int a = 1;
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

			// ���� ī�޶���ġ ���������� �̵�
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
		// ���� �÷��̾���� ������ ������, -> �̰� ������ �� �ְ� �����
		if (ImGui::InputFloat3("Offset Eye", reinterpret_cast<_float*>(&m_FrameNotify[m_iCurrentFrameIndex].vEye)))
		{
			if (false == XMVector4Equal(
				XMLoadFloat4(&m_FrameNotify[m_iCurrentFrameIndex].vEye),
				XMLoadFloat4(&m_FrameNotify[m_iCurrentFrameIndex].vAt)))
			{
				m_SetFrames[m_iCurrentFrameIndex] = true;
				
				_matrix CamMatrix = XMMatrixInverse(nullptr,
					XMMatrixLookAtLH(
						XMLoadFloat4(&m_FrameNotify[m_iCurrentFrameIndex].vEye),
						XMLoadFloat4(&m_FrameNotify[m_iCurrentFrameIndex].vAt),
						XMVectorSet(0.f, 1.f, 0.f, 0.f)));

				m_pToolCameraTrans->Set_WorldMatrix(CamMatrix);

				m_pCurrentModelCom->Set_AnimationFrameCamera(m_iCurrentFrameIndex, m_FrameNotify[m_iCurrentFrameIndex].vEye, m_FrameNotify[m_iCurrentFrameIndex].vAt);
			}
		}

		if (ImGui::InputFloat3("Offset LookAt", reinterpret_cast<_float*>(&m_FrameNotify[m_iCurrentFrameIndex].vAt)))
		{
			if (false == XMVector4Equal(
				XMLoadFloat4(&m_FrameNotify[m_iCurrentFrameIndex].vEye),
				XMLoadFloat4(&m_FrameNotify[m_iCurrentFrameIndex].vAt)))
			{
				m_SetFrames[m_iCurrentFrameIndex] = true;

				_matrix CamMatrix = XMMatrixInverse(nullptr,
					XMMatrixLookAtLH(
						XMLoadFloat4(&m_FrameNotify[m_iCurrentFrameIndex].vEye),
						XMLoadFloat4(&m_FrameNotify[m_iCurrentFrameIndex].vAt),
						XMVectorSet(0.f, 1.f, 0.f, 0.f)));

				m_pToolCameraTrans->Set_WorldMatrix(CamMatrix);

				m_pCurrentModelCom->Set_AnimationFrameCamera(m_iCurrentFrameIndex, m_FrameNotify[m_iCurrentFrameIndex].vEye, m_FrameNotify[m_iCurrentFrameIndex].vAt);
			}
		}

	}

	if (ImGui::Button("Camera Test"))
		m_bTestCamera = true;

	if (m_bTestCamera)
	{
		if (false == m_SetFrames[0])
		{
			MSG_BOX("Camera Notify 0 Frame Not Set");
			m_bTestCamera = false;
		}
		else
		{
			TestCamera();
		}
	}

	return S_OK;
}

HRESULT CAnimationNotify::CameraSetLerp()
{
	ImGui::SeparatorText("Frame Lerps");

	ImGui::SetNextItemWidth(80.f);
	ImGui::InputInt("First Frame", (_int*)&m_iLerpFirstFrameIndex);
	ImGui::SetNextItemWidth(80.f);
	SameLine();
	ImGui::InputInt("Last Frame", (_int*)&m_iLerpLastFrameIndex);

	if (false == ImGui::Button("Notify Camera Lerp"))
		return S_OK;
	
	if (false == m_SetFrames[0])
	{
		MSG_BOX("Camera Notify 0 Frame Not Set");
		return S_OK;
	}

	if (m_iLerpFirstFrameIndex >= m_iLerpLastFrameIndex)
	{
		m_iLerpLastFrameIndex = m_iLerpFirstFrameIndex + 1;
		MSG_BOX("Lerpframe is Greater than Current Frame");
	}
	else if (m_iLerpLastFrameIndex > m_iFrames)
	{
		m_iLerpLastFrameIndex = m_iFrames;
		MSG_BOX("Lerpframe is Smaller than Max Frame");
	}

	_float4 vEye;
	ZEROMEM(&vEye);
	_float4 vAt;
	ZEROMEM(&vAt);
	
	vector<_uint>		m_ChangedFrame;

	_uint iChanged = { 0 };

	for (_uint i  = m_iLerpFirstFrameIndex; i <= m_iLerpLastFrameIndex; ++i)
	{
		// ���� ����� �ε����� ã�´�.
		if (true == m_SetFrames[i])
			m_ChangedFrame.push_back(iChanged);

		++iChanged;
	}

	for (_uint i = 0; i < m_ChangedFrame.size(); ++i)
	{
		if (i == m_ChangedFrame.size() - 1)
		{
			for (_uint j = m_ChangedFrame[i]; j < m_FrameNotify.size(); ++j)
			{
				// ������ �������̸� �������� �� �Ȱ���ä��� ����.
				m_FrameNotify[j] = m_FrameNotify[m_ChangedFrame[i]];
			}
			break;
		}

		NOTIFY PrevNotify = m_FrameNotify[m_ChangedFrame[i]];
		NOTIFY NextNotify = m_FrameNotify[m_ChangedFrame[i + 1]];

		_vector vPrevEye = XMLoadFloat4(&PrevNotify.vEye);
		_vector vNextEye = XMLoadFloat4(&NextNotify.vEye);

		_vector vPrevAt = XMLoadFloat4(&PrevNotify.vAt);
		_vector vNextAt = XMLoadFloat4(&NextNotify.vAt);

		// �������� ó��
		// PrevFrame + ((NextFrame - PrevFrame) * LerpTime / (NextTime - PrevTime))
		for (_uint j = m_ChangedFrame[i]; j <= m_ChangedFrame[i + 1]; ++j)
		{
			_double dTimePercent = m_FrameNotify[j].dTime / (NextNotify.dTime - PrevNotify.dTime);

			XMStoreFloat4(&m_FrameNotify[j].vEye, vPrevEye + (vNextEye - vPrevEye) * (_float)dTimePercent);
			XMStoreFloat4(&m_FrameNotify[j].vAt, vPrevAt + (vNextAt - vPrevAt) * (_float)dTimePercent);

			m_FrameNotify[j].vEye.w = 1.f;
			m_FrameNotify[j].vAt.w = 1.f;
		}

		MSG_BOX("Notify Lerp Success");
	}

	return S_OK;
}

HRESULT CAnimationNotify::LerpReset()
{
	if (ImGui::Button("Lerp Reset"))
	{
		for (auto& frame : m_SetFrames)
		{
			frame = false;
		}
	}
	return S_OK;
}

HRESULT CAnimationNotify::TestCamera()
{
	// �ִϸ��̼ǿ� ���ǵ� ó�� ������ �ϸ�
	// �� Ű�����ӿ� �´� ��Ƽ���̸� �����ͼ� freecam�� �������ִ°� ó���� ���ҵ�.

	if (false == XMVector3Equal(XMLoadFloat4(&m_FrameNotify[m_iCurrentFrameIndex].vEye),
		XMLoadFloat4(&m_FrameNotify[m_iCurrentFrameIndex].vAt)))
	{
		CCamera_Free* pCam = dynamic_cast<CCamera_Free*>(m_pGameInstance->Find_GameObject(LEVEL_TOOL, TEXT("Layer_Tool"), TEXT("GameObject_Camera_Free")));
		
		pCam->Set_CameraView(m_FrameNotify[m_iCurrentFrameIndex].vEye,
			m_FrameNotify[m_iCurrentFrameIndex].vAt,
			_float4(0.f, 1.f, 0.f, 0.f));
	}

	ImGui::SameLine();
	if (ImGui::Button("Camera Reset"))
	{
		m_bTestCamera = false;
		CCamera_Free* pCam = dynamic_cast<CCamera_Free*>(m_pGameInstance->Find_GameObject(LEVEL_TOOL, TEXT("Layer_Tool"), TEXT("GameObject_Camera_Free")));

		pCam->Set_CameraView(_float4(0.f, 2.f, 0.f, 1.f),
			_float4(2.f, 0.f, 2.f, 1.f),
			_float4(0.f, 1.f, 0.f, 0.f));
	}

	return S_OK;
}

HRESULT CAnimationNotify::NotifySpeed()
{
	InputFrameIndex();

	// ���� �������� ���ǵ� ó��
	ImGui::SetNextItemWidth(100.f);
	SameLine();
	if (ImGui::InputFloat("Frame Speed", &m_FrameNotify[m_iCurrentFrameIndex].fSpeed, 1.f, 0.f, "%.2f"))
	{
		if (0 > m_FrameNotify[m_iCurrentFrameIndex].fSpeed)
			m_FrameNotify[m_iCurrentFrameIndex].fSpeed = 0.1f;
		m_pCurrentModelCom->Set_AnimationFrameSpeed(m_iCurrentFrameIndex, m_FrameNotify[m_iCurrentFrameIndex].fSpeed);
	}


	// �ִϸ��̼� ��ü���� ���ǵ尪�� �׷����� ������.
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
	// ���� ������ �ε��� ó��
	if (ImGui::InputInt("Frame Index", (_int*)&m_iCurrentFrameIndex))
	{
		if (0 > m_iCurrentFrameIndex)
			m_iCurrentFrameIndex = 0;
		else if (m_iFrames <= m_iCurrentFrameIndex)
			m_iCurrentFrameIndex = m_iFrames - 1;

		m_pCurrentModelCom->Set_CurrentKeyFrameIndex(m_iCurrentFrameIndex);
	}

	m_iCurrentFrameIndex = m_pCurrentModelCom->Get_CurrentAnimationFrame();
	m_iFrames = m_pCurrentModelCom->Get_AnimationFrames();

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