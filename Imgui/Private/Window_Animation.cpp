#include "..\Public\Window_Animation.h"
#include "AnimModel.h"

CWindow_Animation::CWindow_Animation()
{
}

void CWindow_Animation::Set_CurrentAnimModel(CAnimModel* pAnimModel)
{
	m_pCurrentAnimModel = pAnimModel;
}

HRESULT CWindow_Animation::Initialize(void* pArg)
{

	return S_OK;
}

void CWindow_Animation::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
	ImGui::Begin("Animation", nullptr, m_WindowFlag);

	AnimationIndex();

	ImGui::End();
}

HRESULT CWindow_Animation::AnimationIndex()
{
	if (nullptr == m_pCurrentAnimModel)
		return S_OK;
	_char szNum[MAX_STR] = "";
	_itoa_s(m_pCurrentAnimModel->Get_NumAnimations() - 1, szNum, MAX_STR, 10);

	_int iIndex = m_pCurrentAnimModel->Get_PreToolAnimationIndex();

	SetNextItemWidth(100.f);
	if (ImGui::InputInt("Animation Index", &iIndex))
	{
		if (m_pCurrentAnimModel->Get_NumAnimations() - 1 < (_uint)iIndex ||
			0 > iIndex)
		{
			return E_FAIL;
		}
		else
		{
			m_iAnimationIndex = iIndex;
			m_pCurrentAnimModel->Set_AnimIndex(m_iAnimationIndex);
			m_pCurrentAnimModel->Set_PreToolAnimationIndex(m_iAnimationIndex);
		}
	}
	SameLine();
	ImGui::Text("Max : ");
	SameLine();
	ImGui::TextColored(ImVec4(1.f, 0.f, 1.f, 1.f), szNum);

	AnimationSpeed();
	return S_OK;
}

HRESULT CWindow_Animation::AnimationSpeed()
{
	m_dAnimationSpeed = m_pCurrentAnimModel->Get_PreToolAnimationSpeed();
	if (ImGui::InputDouble("Animation Speed", &m_dAnimationSpeed))
	{
		m_pCurrentAnimModel->Set_AnimationSpeed(m_dAnimationSpeed);
		m_pCurrentAnimModel->Set_PreToolAnimationSpeed(m_dAnimationSpeed);
	}

	return S_OK;
}

CWindow_Animation* CWindow_Animation::Create(void* pArg)
{
	CWindow_Animation* pInstance = New CWindow_Animation();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CWindow_Animation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWindow_Animation::Free()
{
	__super::Free();
}
