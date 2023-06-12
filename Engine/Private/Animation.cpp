#include "..\Public\Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

HRESULT CAnimation::Initialize(const aiAnimation* pAIAnimation)
{
	strcpy_s(m_szName, MAX_PATH, pAIAnimation->mName.data);
	m_iNumChannels = pAIAnimation->mNumChannels;
	m_Duration = pAIAnimation->mDuration;
	m_TickPerSecond = pAIAnimation->mTicksPerSecond;

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CChannel* pChannel = CChannel::Create(pAIAnimation->mChannels[i]);

		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

void CAnimation::Invalidate_TransformationMatrix(_double dTimeDelta)
{
	m_TimeAcc += m_TickPerSecond * dTimeDelta;

	/* ���� ����� �ð��� �µ��� ��� ���� ���¸� Ű������������ ��������Ͽ� �����Ѵ�. */
	for (auto& pChannel : m_Channels)
	{
		if (nullptr == pChannel)
			return;

		pChannel->Invalidate_TransformationMatrix(m_TimeAcc);
	}
}

CAnimation* CAnimation::Create(const aiAnimation* pAIAnimation)
{
	CAnimation* pInstance = new CAnimation();
	if (FAILED(pInstance->Initialize(pAIAnimation)))
	{
		MSG_BOX("Failed to Created CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimation::Free()
{
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);
	m_Channels.clear();
}
