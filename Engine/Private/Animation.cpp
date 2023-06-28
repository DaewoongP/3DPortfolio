#include "..\Public\Animation.h"
#include "Channel.h"
#include "Camera.h"

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
	, m_isLoop(rhs.m_isLoop)
	, m_isPaused(rhs.m_isPaused)
	, m_iMaxFrameChannelIndex(rhs.m_iMaxFrameChannelIndex)
	, m_iAnimationFrames(rhs.m_iAnimationFrames)
	, m_AnimationNotify(rhs.m_AnimationNotify)
{
	lstrcpy(m_szName, rhs.m_szName);

	for (auto& pChannel : m_Channels)
	{
		Safe_AddRef(pChannel);
	}
}

void CAnimation::Set_CurrentKeyFrameIndex(CModel::BONES& Bones, _uint iKeyFrameIndex)
{
	_uint iChannelIndex = { 0 };

	for (auto& pChannel : m_Channels)
	{
		if (nullptr == pChannel)
			return;
		m_ChannelCurrentKeyFrames[iChannelIndex] = iKeyFrameIndex;
		
		pChannel->Invalidate_TransformationMatrix(Bones, 
			pChannel->Get_CurrentKeyFrameTime(iKeyFrameIndex),
			&m_ChannelCurrentKeyFrames[iChannelIndex]);

		++iChannelIndex;
	}
}

HRESULT CAnimation::Initialize(Engine::ANIMATION* pAnimation, const CModel::BONES& Bones)
{
	m_isLoop = true;

	// 애니메이션 정보 저장
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

	_uint iNumKeyFrame = { 0 };
	_uint iChannelIndex = { 0 };
	// 채널을 순회하면서 모든 채널중 가장 큰 키프레임을 찾아 해당 채널의 인덱스와 해당 프레임을 저장.
	for (auto& pChannel : m_Channels)
	{
		if (pChannel->Get_NumKeyFrames() > iNumKeyFrame)
		{
			iNumKeyFrame = pChannel->Get_NumKeyFrames();
			m_iMaxFrameChannelIndex = iChannelIndex;
		}

		++iChannelIndex;
	}
	m_iAnimationFrames = iNumKeyFrame;

	// 노티파이 벡터 초기화
	m_AnimationNotify.clear();

	for (_uint i = 0; i < m_iAnimationFrames; ++i)
	{
		NOTIFY Notify;
		Notify.vEye  = _float4(0.f, 0.f, 0.f, 1.f);
		Notify.vAt = _float4(0.f, 0.f, 0.f, 1.f);
		Notify.fSpeed = 1.f;
		Notify.dTime = m_Channels[m_iMaxFrameChannelIndex]->Get_CurrentKeyFrameTime(i);

		m_AnimationNotify.push_back(Notify);
	}

	return S_OK;
}

ANIMATIONFLAG CAnimation::Invalidate_TransformationMatrix(CModel::BONES& Bones, _double TimeDelta)
{
	if (m_isPaused)
		return ANIM_PAUSED;

	// 프레임 별로 애니메이션 스피드 제어.
	if (m_AnimationNotify.size() > 0)
		m_dTimeAcc += m_dTickPerSecond * TimeDelta * m_AnimationNotify[m_ChannelCurrentKeyFrames[m_iMaxFrameChannelIndex]].fSpeed * 1.05;
	else
		m_dTimeAcc += m_dTickPerSecond * TimeDelta * 1.05;

	if (m_dTimeAcc >= m_dDuration)
	{
		if (true == m_isLoop)
		{
			m_dTimeAcc = 0.f;
		}
		else
			return ANIM_FINISHED;
	}

	_uint		iChannelIndex = 0;

	for (auto& pChannel : m_Channels)
	{
		if (nullptr == pChannel)
			return ANIM_END;

		pChannel->Invalidate_TransformationMatrix(Bones, m_dTimeAcc, &m_ChannelCurrentKeyFrames[iChannelIndex++]);
	}

	return ANIM_PLAYING;
}

void CAnimation::Invalidate_Camera(CCamera* pCamera, CTransform* pPlayerTransform, _double dTimeDelta)
{
	// 카메라 컴포넌트를 받아서 처리.
	// 현재 노티파이에있는 카메라 상태를 처리해준다.
	NOTIFY Notify = m_AnimationNotify[m_ChannelCurrentKeyFrames[m_iMaxFrameChannelIndex]];

	if (XMVectorGetX(XMVectorEqual(XMLoadFloat4(&Notify.vEye), XMLoadFloat4(&Notify.vAt))))
		return;

	_vector vCamPos = pCamera->Get_TransformState(CTransform::STATE_POSITION);
	_vector vCamDir = pCamera->Get_TransformState(CTransform::STATE_LOOK);
	_vector vPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	_vector vNotifyDir = XMLoadFloat4(&Notify.vAt) - XMLoadFloat4(&Notify.vEye);

	_matrix InvalidateMatrix = 
		XMMatrixInverse(nullptr, 
			XMMatrixLookAtLH(vCamPos + XMLoadFloat4(&Notify.vEye), vCamPos + XMLoadFloat4(&Notify.vAt) + XMVector3Normalize(vCamDir + vNotifyDir), XMVectorSet(0.f, 1.f, 0.f, 0.f)));

	pCamera->Set_CameraWorldMatrix(InvalidateMatrix);
}

HRESULT CAnimation::SetUp_AnimationNotifies(vector<NOTIFY> Notifies)
{
	if (Notifies.size() != m_AnimationNotify.size())
	{
		MSG_BOX("Failed Notify size");
		
		return E_FAIL;
	}

	m_AnimationNotify.clear();
	m_AnimationNotify = Notifies;

	return S_OK;
}

ANIMATIONFLAG CAnimation::Lerp_TransformMatrix(CModel::BONES& Bones, CAnimation* pCurrentAnimation, _double TimeDelta)
{
	_double dDuration = 0.2;

	m_dTimeAcc += TimeDelta;

	if (m_dTimeAcc >= dDuration) // 선형보간 시간 대입
	{
		return ANIM_LERP_FINISHED;
	}

	for (auto& pPrevChannel : m_Channels)
	{
		if (nullptr == pPrevChannel)
			return ANIM_END;

		for (auto& pCurChannel : pCurrentAnimation->m_Channels)
		{
			if (nullptr == pCurChannel)
				return ANIM_END;

			if (pPrevChannel->Get_BoneIndex() == pCurChannel->Get_BoneIndex())
				pPrevChannel->Lerp_TransformationMatrix(Bones, pCurChannel, dDuration, m_dTimeAcc, m_ChannelCurrentKeyFrames[m_iMaxFrameChannelIndex]);
		}
	}

	return ANIM_LERP;
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