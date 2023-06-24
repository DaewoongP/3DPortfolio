#include "..\Public\Window_ObjectOptions.h"
#include "AnimModel.h"
#include "NonAnimModel.h"
#include "ImWindow_Manager.h"
#include "AnimationNotify.h"

CWindow_ObjectOptions::CWindow_ObjectOptions()
{
}

void CWindow_ObjectOptions::Set_CurrentDummy(CDummy::DUMMYTYPE eDummyType, CDummy* pDummy)
{
	if (nullptr == pDummy)
		return;

	m_eCurrentDummyType = eDummyType;

	m_pCurrentDummy = pDummy;

	m_pCurrentModel = dynamic_cast<CModel*>(pDummy->Find_Component(TEXT("Com_Model")));

	m_iNumAnimations = m_pCurrentModel->Get_NumAnimations();
}

HRESULT CWindow_ObjectOptions::Initialize(void* pArg)
{
	m_FrameSpeedFunc = Funcs::FrameSpeedSaw;

	m_pAnimationNotify = CAnimationNotify::Create();

	ZeroMemory(m_ColliderExtents, sizeof(_float3) * 3);

	return S_OK;
}

void CWindow_ObjectOptions::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	ImGui::Begin("Object Options", nullptr, m_WindowFlag);

	if (ImGui::BeginTabBar("ObjectOptionsTab"))
	{
		if (ImGui::BeginTabItem("Animation"))
		{
			if (nullptr != m_pCurrentModel &&
				CDummy::DUMMY_ANIM == m_eCurrentDummyType)
			{
				AnimationIndex();

				AnimationPause();

				AnimationSpeed(dTimeDelta);
			}

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Collider"))
		{
			if (nullptr != m_pCurrentModel &&
				CDummy::DUMMY_END != m_eCurrentDummyType)
			{
				AddCollider();
			}

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}

HRESULT CWindow_ObjectOptions::AnimationIndex()
{
	_char szNum[MAX_STR] = "";
	// 인덱스이므로 전체 개수 - 1
	_itoa_s(m_iNumAnimations - 1, szNum, MAX_STR, 10);

	_int iIndex = m_pCurrentDummy->Get_PreToolAnimationIndex();
	WCharToChar(m_pCurrentModel->Get_AnimationName(), m_szAnimationName);

	m_iAnimationFrames = m_pCurrentModel->Get_AnimationFrames();

	m_iCurrentAnimationFrame = m_pCurrentModel->Get_CurrentAnimationFrame();

	for (_uint i = 0; i < m_iAnimationFrames; ++i)
		m_FrameSpeeds.push_back(1.f);

	SetNextItemWidth(100.f);
	if (ImGui::InputInt("Animation Index", &iIndex))
	{
		// 애니메이션 인덱스가 변경되면
		// 애니메이션 스피드 벡터 클리어
		m_FrameSpeeds.clear();
		// 애니메이션 프레임 개수만큼 1로 초기화
		for (_uint i = 0; i < m_iAnimationFrames; ++i)
			m_FrameSpeeds.push_back(1.f);
		// 정지버튼 비활성화
		m_bPauseButton = false;
		m_pCurrentModel->Set_AnimationPause(false);
		// 
		if (m_iNumAnimations - 1 < (_uint)iIndex ||
			0 > iIndex)
			return E_FAIL;
		else
		{
			m_iAnimationIndex = iIndex;
			m_pCurrentModel->Set_AnimIndex(m_iAnimationIndex);
			m_pCurrentDummy->Set_PreToolAnimationIndex(m_iAnimationIndex);
		}
	}

	SameLine();
	ImGui::Text("Max : ");

	SameLine();
	ImGui::TextColored(ImVec4(1.f, 0.f, 1.f, 1.f), szNum);
	return S_OK;
}

HRESULT CWindow_ObjectOptions::AnimationPause()
{
	if (!m_bPauseButton)
	{
		if (ImGui::Button("Pause"))
		{
			m_bPauseButton = true;
			m_pCurrentModel->Set_AnimationPause(true);
		}
	}
	else
	{
		if (ImGui::Button("Start"))
		{
			m_bPauseButton = false;
			m_pCurrentModel->Set_AnimationPause(false);
		}
	}

	SameLine();
	
	ImGui::Text(m_szAnimationName);

	return S_OK;
}

HRESULT CWindow_ObjectOptions::AnimationSpeed(_double dTimeDelta)
{
	_float progress = (_float)m_iCurrentAnimationFrame / (m_iAnimationFrames - 1);
	ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
	ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
	ImGui::Text("Animation Progress");

	_char buf[MAX_STR] = "";
	sprintf_s(buf, "%d/%d", m_iCurrentAnimationFrame, (m_iAnimationFrames - 1));
	ImGui::ProgressBar(progress, ImVec2(0.f, 0.f), buf);

	if (m_bPauseButton)
	{
		ImGui::SetNextItemWidth(100.f);
		// 현재 프레임 인덱스 처리
		if (ImGui::InputInt("Frame Index", (_int*)&m_iCurrentAnimationFrame))
		{
			if (0 > m_iCurrentAnimationFrame)
				m_iCurrentAnimationFrame = 0;
			else if (m_iAnimationFrames - 1 <= m_iCurrentAnimationFrame)
				m_iCurrentAnimationFrame = m_iAnimationFrames - 1;
			m_pCurrentModel->Set_CurrentKeyFrameIndex(m_iCurrentAnimationFrame);
		}
		// 현재 프레임의 스피드 처리
		ImGui::SetNextItemWidth(100.f);
		SameLine();
		if (ImGui::InputFloat("Frame Speed", &m_FrameSpeeds[m_iCurrentAnimationFrame], 1.f, 0.f, "%.2f"))
		{
			if (0 > m_FrameSpeeds[m_iCurrentAnimationFrame])
				m_FrameSpeeds[m_iCurrentAnimationFrame] = 0.1f;
			m_pCurrentModel->Set_AnimationFrameSpeed(m_iCurrentAnimationFrame, m_FrameSpeeds[m_iCurrentAnimationFrame]);
		}
	}
	// 해당 애니메이션의 특정 프레임에서 처리할 정보들
	if (nullptr != m_pCurrentModel && 
		nullptr != m_pCurrentDummy &&
		CDummy::DUMMY_ANIM == m_eCurrentDummyType &&
		0 < m_iNumAnimations &&
		0 < m_iAnimationFrames &&
		true == m_bPauseButton)
	{
		MODELWINDOW->Set_Picking(false);

		AnimationNotify(dTimeDelta);
	}
	

	// 애니메이션 전체적인 스피드값을 그래프로 보여줌.
	ImGui::PlotHistogram("Animation Speeds", m_FrameSpeedFunc, &m_FrameSpeeds, m_iAnimationFrames, 0, nullptr, 0.f, 10.f, ImVec2(0, 100));

	return S_OK;
}

HRESULT CWindow_ObjectOptions::AnimationNotify(_double dTimeDelta)
{
	m_pAnimationNotify->Set_CurrentIndex(m_iAnimationFrames, m_iCurrentAnimationFrame);
	m_pAnimationNotify->Set_CurrentAnimationObject(static_cast<CAnimModel*>(m_pCurrentDummy), m_pCurrentModel);

	m_pAnimationNotify->Tick(dTimeDelta);

	return S_OK;
}

HRESULT CWindow_ObjectOptions::AddCollider()
{
	ImGui::InputFloat3("Extents X", reinterpret_cast<_float*>(&m_ColliderExtents[0]));
	ImGui::InputFloat3("Extents Y", reinterpret_cast<_float*>(&m_ColliderExtents[1]));
	ImGui::InputFloat3("Extents Z", reinterpret_cast<_float*>(&m_ColliderExtents[2]));

	return S_OK;
}

CWindow_ObjectOptions* CWindow_ObjectOptions::Create(void* pArg)
{
	CWindow_ObjectOptions* pInstance = New CWindow_ObjectOptions();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CWindow_ObjectOptions");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWindow_ObjectOptions::Free()
{
	__super::Free();

	Safe_Release(m_pAnimationNotify);
}
