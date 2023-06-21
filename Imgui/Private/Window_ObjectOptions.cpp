#include "..\Public\Window_ObjectOptions.h"
#include "AnimModel.h"
#include "NonAnimModel.h"

CWindow_ObjectOptions::CWindow_ObjectOptions()
{
}

void CWindow_ObjectOptions::Set_CurrentDummy(CDummy::DUMMYTYPE eDummyType, CDummy* pDummy)
{
	m_eCurrentDummyType = eDummyType;
	m_pCurrentDummy = pDummy;
}

HRESULT CWindow_ObjectOptions::Initialize(void* pArg)
{
	m_FrameSpeedFunc = Funcs::FrameSpeedSaw;
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
			if (nullptr != m_pCurrentDummy &&
				CDummy::DUMMY_ANIM == m_eCurrentDummyType)
			{
				AnimationIndex();

				AnimationPause();

				AnimationSpeed();
			}

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Collider"))
		{
			if (nullptr != m_pCurrentDummy &&
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
	_itoa_s(static_cast<CAnimModel*>(m_pCurrentDummy)->Get_NumAnimations() - 1, szNum, MAX_STR, 10);

	_int iIndex = static_cast<CAnimModel*>(m_pCurrentDummy)->Get_PreToolAnimationIndex();

	WCharToChar(static_cast<CAnimModel*>(m_pCurrentDummy)->Get_AnimationName(), m_szAnimationName);

	m_iAnimationMaxKeyFrames = static_cast<CAnimModel*>(m_pCurrentDummy)->Get_MaxKeyFrame();
	m_iCurrentAnimationFrameIndex = static_cast<CAnimModel*>(m_pCurrentDummy)->Get_CurrentMaxChannelKeyFrameIndex();

	for (_uint i = 0; i < m_iAnimationMaxKeyFrames; ++i)
		m_FrameSpeeds.push_back(1.f);

	SetNextItemWidth(100.f);
	if (ImGui::InputInt("Animation Index", &iIndex))
	{
		m_FrameSpeeds.clear();
		for (_uint i = 0; i < m_iAnimationMaxKeyFrames; ++i)
			m_FrameSpeeds.push_back(1.f);

		m_bPauseButton = false;
		static_cast<CAnimModel*>(m_pCurrentDummy)->Set_AnimationPause(false);

		if (static_cast<CAnimModel*>(m_pCurrentDummy)->Get_NumAnimations() - 1 < (_uint)iIndex ||
			0 > iIndex)
			return E_FAIL;
		else
		{
			m_iAnimationIndex = iIndex;
			static_cast<CAnimModel*>(m_pCurrentDummy)->Set_AnimIndex(m_iAnimationIndex);
			static_cast<CAnimModel*>(m_pCurrentDummy)->Set_PreToolAnimationIndex(m_iAnimationIndex);
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
			static_cast<CAnimModel*>(m_pCurrentDummy)->Set_AnimationPause(true);
		}
	}
	else
	{
		if (ImGui::Button("Start"))
		{
			m_bPauseButton = false;
			static_cast<CAnimModel*>(m_pCurrentDummy)->Set_AnimationPause(false);
		}
	}

	SameLine();
	
	ImGui::Text(m_szAnimationName);

	return S_OK;
}

HRESULT CWindow_ObjectOptions::AnimationSpeed()
{
	_float progress = (_float)m_iCurrentAnimationFrameIndex / (m_iAnimationMaxKeyFrames - 1);
	ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
	ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
	ImGui::Text("Animation Progress");

	_char buf[MAX_STR] = "";
	sprintf_s(buf, "%d/%d", m_iCurrentAnimationFrameIndex, (m_iAnimationMaxKeyFrames - 1));
	ImGui::ProgressBar(progress, ImVec2(0.f, 0.f), buf);

	if (m_bPauseButton)
	{
		if (ImGui::InputInt("Frame Index", (_int*)&m_iCurrentAnimationFrameIndex))
		{
			if (0 > m_iCurrentAnimationFrameIndex)
				m_iCurrentAnimationFrameIndex = 0;
			else if (m_iAnimationMaxKeyFrames - 1 <= m_iCurrentAnimationFrameIndex)
				m_iCurrentAnimationFrameIndex = m_iAnimationMaxKeyFrames - 1;
			static_cast<CAnimModel*>(m_pCurrentDummy)->Set_CurrentKeyFrameIndex(m_iCurrentAnimationFrameIndex);
		}
		if (ImGui::InputFloat("Frame Speed", &m_FrameSpeeds[m_iCurrentAnimationFrameIndex], 1.f, 0.f, "%.2f"))
		{
			if (0 > m_FrameSpeeds[m_iCurrentAnimationFrameIndex])
				m_FrameSpeeds[m_iCurrentAnimationFrameIndex] = 0.1f;
			static_cast<CAnimModel*>(m_pCurrentDummy)->Set_AnimationFrameSpeed(m_iCurrentAnimationFrameIndex, m_FrameSpeeds[m_iCurrentAnimationFrameIndex]);
		}
	}


	ImGui::PlotHistogram("Animation Speeds", m_FrameSpeedFunc, &m_FrameSpeeds, m_iAnimationMaxKeyFrames, 0, nullptr, 0.f, 10.f, ImVec2(0, 100));

	return S_OK;
}

HRESULT CWindow_ObjectOptions::AddCollider()
{
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
}
