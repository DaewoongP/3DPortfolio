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
	m_pCurrentAnimationCom = m_pCurrentModelCom->Get_Animation();

	m_FrameNotify = m_pCurrentAnimationCom->Get_CurrentAnimationNotify();
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
	// 사운드, 파티클, 이펙트, 카메라 등등의 처리
	if (nullptr != m_pCurrentAnimationCom && ImGui::BeginTabBar("Notify"))
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

		if (ImGui::BeginTabItem("Notify Save & Load"))
		{
			NotifySaveLoad();

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
		if (ImGui::InputFloat3("Offset CamPos", reinterpret_cast<_float*>(&m_FrameNotify[m_iCurrentFrameIndex].vEye)))
		{
			if (m_iCurrentFrameIndex == m_iFrames - 1)
				m_iCurrentFrameIndex -= 1;

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

				m_pCurrentAnimationCom->Set_FrameCamera(m_iCurrentFrameIndex, m_FrameNotify[m_iCurrentFrameIndex].vEye, m_FrameNotify[m_iCurrentFrameIndex].vAt);
			}
		}

		if (ImGui::InputFloat3("Offset CamDir", reinterpret_cast<_float*>(&m_FrameNotify[m_iCurrentFrameIndex].vAt)))
		{
			if (m_iCurrentFrameIndex == m_iFrames - 1)
				m_iCurrentFrameIndex -= 1;

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

				m_pCurrentAnimationCom->Set_FrameCamera(m_iCurrentFrameIndex, m_FrameNotify[m_iCurrentFrameIndex].vEye, m_FrameNotify[m_iCurrentFrameIndex].vAt);
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
	else if (m_iLerpLastFrameIndex >= m_iFrames - 1)
	{
		m_iLerpLastFrameIndex = m_iFrames - 2;
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
		// 값이 적용된 인덱스를 찾는다.
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
				// 마지막 프레임이면 나머지를 걍 똑같이채우고 나감.
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

		// 선형보간 처리
		// PrevFrame + ((NextFrame - PrevFrame) * LerpTime / (NextTime - PrevTime))
		for (_uint j = m_ChangedFrame[i]; j <= m_ChangedFrame[i + 1]; ++j)
		{
			_double dTimePercent = m_FrameNotify[j].dTime / (NextNotify.dTime - PrevNotify.dTime);

			XMStoreFloat4(&m_FrameNotify[j].vEye, vPrevEye + (vNextEye - vPrevEye) * (_float)dTimePercent);
			XMStoreFloat4(&m_FrameNotify[j].vAt, vPrevAt + (vNextAt - vPrevAt) * (_float)dTimePercent);

			m_FrameNotify[j].vEye.w = 1.f;
			m_FrameNotify[j].vAt.w = 1.f;
		}


		wstring MsgIndex;
		_tchar szPrevFrameIndex[MAX_PATH] = TEXT("");
		_tchar szNextFrameIndex[MAX_PATH] = TEXT("");
		_itow_s(m_ChangedFrame[i], szPrevFrameIndex, MAX_PATH, 10);
		_itow_s(m_ChangedFrame[i + 1], szNextFrameIndex, MAX_PATH, 10);

		MsgIndex = szPrevFrameIndex;
		MsgIndex += TEXT(" ~ ");
		MsgIndex += szNextFrameIndex;
		MsgIndex += TEXT("Notify Lerp Success");

		MessageBox(nullptr, MsgIndex.c_str(), L"System Message", MB_OK);
	}

	return S_OK;
}

HRESULT CAnimationNotify::LerpReset()
{
	if (ImGui::Button("Lerp Reset"))
	{
		for (auto frame : m_SetFrames)
			frame = false;
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

	// 현재 프레임의 스피드 처리
	ImGui::SetNextItemWidth(100.f);
	SameLine();
	if (ImGui::InputFloat("Frame Speed", &m_FrameNotify[m_iCurrentFrameIndex].fSpeed, 1.f, 0.f, "%.2f"))
	{
		if (0 > m_FrameNotify[m_iCurrentFrameIndex].fSpeed)
			m_FrameNotify[m_iCurrentFrameIndex].fSpeed = 0.1f;

		m_pCurrentAnimationCom->Set_FrameSpeed(m_iCurrentFrameIndex, m_FrameNotify[m_iCurrentFrameIndex].fSpeed);
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
		else if (m_iFrames <= m_iCurrentFrameIndex)
			m_iCurrentFrameIndex = m_iFrames - 1;

		m_pCurrentModelCom->Set_CurrentKeyFrameIndex(m_iCurrentFrameIndex);
	}

	m_iCurrentFrameIndex = m_pCurrentAnimationCom->Get_CurrentAnimationFrame();
	m_iFrames = m_pCurrentAnimationCom->Get_AnimationFrames();

	return S_OK;
}

HRESULT CAnimationNotify::NotifySaveLoad()
{
	ImGui::PushID(0);
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(2 / 7.0f, 0.6f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(2 / 7.0f, 0.7f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(2 / 7.0f, 0.8f, 0.8f));
	if (ImGui::Button("Notify Save"))
	{
		// Pick Disable

		IMFILE->OpenDialog("SaveNotifyDialog", "Choose Folder", ".Notify", "Notify.Notify");
	}
	ImGui::PopStyleColor(3);
	ImGui::PopID();

	NotifySaveButton();

	ImGui::SameLine();
	ImGui::PushID(0);
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.6f, 1.0f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.7f, 1.0f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.8f, 1.0f, 0.8f));

	if (ImGui::Button("Notify Load"))
	{
		// Pick Disable

		IMFILE->OpenDialog("LoadNotifyDialog", "Choose File", ".Notify", ".");
	}

	ImGui::PopStyleColor(3);
	ImGui::PopID();

	NotifyLoadButton();

	return S_OK;
}

HRESULT CAnimationNotify::NotifySaveButton()
{
	// display
	if (IMFILE->Display("SaveNotifyDialog"))
	{
		// action if OK
		if (IMFILE->IsOk())
		{
			string filePath = IMFILE->GetFilePathName();
			_tchar wszPath[MAX_PATH] = TEXT("");
			CharToWChar(filePath.c_str(), wszPath);
			if (FAILED(NotifyWrite_File(wszPath)))
				MSG_BOX("Failed File Write");
		}

		// close
		IMFILE->Close();
	}

	return S_OK;
}

HRESULT CAnimationNotify::NotifyWrite_File(const _tchar* pPath)
{
	m_iAnimationIndex = m_pCurrentAnimDummy->Get_PreToolAnimationIndex();
	m_iFrames = m_pCurrentAnimationCom->Get_AnimationFrames();

	HANDLE hFile = CreateFile(pPath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong	dwByte = 0;

	// Current Animation Index
	WriteFile(hFile, &m_iAnimationIndex, sizeof(_uint), &dwByte, nullptr);
	// Animation's NumFrames
	WriteFile(hFile, &m_iFrames, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < m_iFrames; ++i)
	{
		// Notify Save
		WriteFile(hFile, &m_FrameNotify[i], sizeof(NOTIFY), &dwByte, nullptr);
	}

	CloseHandle(hFile);

	MSG_BOX("File Save Success");

	return S_OK;
}

HRESULT CAnimationNotify::NotifyLoadButton()
{
	// display
	if (IMFILE->Display("LoadNotifyDialog"))
	{
		// action if OK
		if (IMFILE->IsOk())
		{
			map<string, string> strMap = IMFILE->GetSelection();
			string filePathName = IMFILE->GetFilePathName();
			_tchar wszName[MAX_PATH] = TEXT("");
			CharToWChar(filePathName.c_str(), wszName);
			if (FAILED(NotifyRead_File(wszName)))
				MSG_BOX("Failed File Read");
		}

		// close
		IMFILE->Close();
	}

	return S_OK;
}

HRESULT CAnimationNotify::NotifyRead_File(const _tchar* pFileName)
{
	HANDLE hFile = CreateFile(pFileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong	dwByte = 0;

	// Current Animation Index
	ReadFile(hFile, &m_iAnimationIndex, sizeof(_uint), &dwByte, nullptr);
	// Animation's NumFrames
	ReadFile(hFile, &m_iFrames, sizeof(_uint), &dwByte, nullptr);

	m_FrameNotify.clear();
	m_SetFrames.clear();
	for (_uint i = 0; i < m_iFrames; ++i)
	{
		NOTIFY Notify;
		// Notify Save
		ReadFile(hFile, &Notify, sizeof(NOTIFY), &dwByte, nullptr);

		m_FrameNotify.push_back(Notify);
		m_SetFrames.push_back(true);
	}

	m_pCurrentModelCom->Set_AnimIndex(m_iAnimationIndex);
	m_pCurrentAnimDummy->Set_PreToolAnimationIndex(m_iAnimationIndex);

	CloseHandle(hFile);

	MSG_BOX("File Load Success");

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