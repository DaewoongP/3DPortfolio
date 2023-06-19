#include "..\Public\Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& rhs)
	: m_iNumChannels(rhs.m_iNumChannels)
	, m_Channels(rhs.m_Channels)
	, m_ChannelCurrentKeyFrames(rhs.m_ChannelCurrentKeyFrames)
	, m_dDuration(rhs.m_dDuration)
	, m_dOriginTickPerSecond(rhs.m_dOriginTickPerSecond)
	, m_dTickPerSecond(rhs.m_dTickPerSecond)
	, m_dTimeAcc(rhs.m_dTimeAcc)
	, m_bIsLoop(rhs.m_bIsLoop)
{
	lstrcpy(m_szName, rhs.m_szName);

	for (auto& pChannel : m_Channels)
	{
		Safe_AddRef(pChannel);
	}
}

void CAnimation::Set_FrameSpeed(_uint iFrameIndex, _float fSpeed)
{
	m_FrameSpeeds[iFrameIndex] = fSpeed;
}

_uint CAnimation::Get_MaxKeyFrameInAnimationChannels()
{
	_uint iNumKeyFrame = { 0 };
	_uint iChannelIndex = { 0 };
	for (auto& pChannel : m_Channels)
	{
		if (pChannel->Get_NumKeyFrames() > iNumKeyFrame)
		{
			iNumKeyFrame = pChannel->Get_NumKeyFrames();
			m_iMaxNumKeyFrameChannelIndex = iChannelIndex;
		}
		++iChannelIndex;
	}
	
	for (_uint i = 0; i < m_Channels[m_iMaxNumKeyFrameChannelIndex]->Get_NumKeyFrames(); ++i)
		m_FrameSpeeds.push_back(1.f);

	return iNumKeyFrame;
}

_uint CAnimation::Get_CurrentMaxChannelKeyFrameIndex()
{
	return m_ChannelCurrentKeyFrames[m_iMaxNumKeyFrameChannelIndex];
}

HRESULT CAnimation::Initialize(Engine::ANIMATION* pAnimation, const CModel::BONES& Bones)
{
	m_bIsLoop = true;

	lstrcpy(m_szName, pAnimation->Name);
	m_dDuration = pAnimation->Duration;

	m_dOriginTickPerSecond = pAnimation->TickPerSecond;
	m_dTickPerSecond = pAnimation->TickPerSecond;
	
	m_iNumChannels = pAnimation->NumChannels;

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		CChannel* pChannel = CChannel::Create(pAnimation->Channels[i], Bones);

		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	m_ChannelCurrentKeyFrames.resize(m_iNumChannels);

	return S_OK;
}

void CAnimation::Invalidate_TransformationMatrix(CModel::BONES& Bones, _double TimeDelta)
{
	if (m_bIsPaused)
		return;

	if (m_FrameSpeeds.size() > 0)
		m_dTimeAcc += m_dTickPerSecond * TimeDelta * m_FrameSpeeds[m_ChannelCurrentKeyFrames[m_iMaxNumKeyFrameChannelIndex]];
	else
		m_dTimeAcc += m_dTickPerSecond * TimeDelta;

	if (m_dTimeAcc >= m_dDuration)
	{
		if (true == m_bIsLoop)
		{
			m_dTimeAcc = 0.f;
		}
	}

	_uint		iChannelIndex = 0;
	for (auto& pChannel : m_Channels)
	{
		if (nullptr == pChannel)
			return;

		pChannel->Invalidate_TransformationMatrix(Bones, m_dTimeAcc, &m_ChannelCurrentKeyFrames[iChannelIndex++]);
	}
}

CAnimation* CAnimation::Create(Engine::ANIMATION* pAnimation, const CModel::BONES& Bones)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pAnimation, Bones)))
	{
		MSG_BOX("Failed to Created CAnimation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CAnimation* CAnimation::Clone()
{
	return new CAnimation(*this);
}

void CAnimation::Free()
{
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();

}