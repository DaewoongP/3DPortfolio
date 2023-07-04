#pragma once
#include "Model.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	explicit CAnimation();
	explicit CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	// 현재 애니메이션 이름 반환
	const _tchar* Get_AnimationName() const { return m_szName; }
	// 애니메이션의 프레임 개수 반환
	_uint Get_AnimationFrames() { return m_iAnimationFrames; }
	// 채널 중 키프레임 최대치 기준, **현재 애니메이션의 프레임 반환**
	_uint Get_CurrentAnimationFrame() { return m_ChannelCurrentKeyFrames[m_iMaxFrameChannelIndex]; }
	vector<NOTIFY> Get_CurrentAnimationNotify() { return m_AnimationNotify; }
	// 프레임 인덱스에 해당하는 스피드값 설정.
	void Set_FrameSpeed(_uint iFrameIndex, _float fSpeed) {
		m_AnimationNotify[iFrameIndex].fSpeed = fSpeed;
	}
	// 프레임 인덱스에 해당하는 노티파이 카메라 값 설정
	void Set_FrameCamera(_uint iFrameIndex, _float4 vEye, _float4 vAt) {
		m_AnimationNotify[iFrameIndex].vEye = vEye;
		m_AnimationNotify[iFrameIndex].vAt = vAt;
	}
	void Set_CurrentKeyFrameIndex(CModel::BONES& Bones, _uint iKeyFrameIndex);
	void Set_Pause(_bool isPause) { m_isPaused = isPause; }
	void Set_TickPerSecond(_double dMultiply) 
	{
		if (0 >= dMultiply)
			return;

		m_dTickPerSecond = m_dOriginTickPerSecond * dMultiply;
	}
	void Set_Loop(_bool isLoop) { m_isLoop = isLoop; }
	// 애니메이션을 초기화 하는 함수
	void Reset() 
	{ 
		for (auto& iCurrentKeyFrame : m_ChannelCurrentKeyFrames)
			iCurrentKeyFrame = 0;
		m_dTimeAcc = 0.0;
	}
	void TimeAccReset() { m_dTimeAcc = 0.0; }

public:
	HRESULT Initialize(Engine::ANIMATION* pAnimation, const CModel::BONES& Bones);
	ANIMATIONFLAG Invalidate_TransformationMatrix(CModel::BONES& Bones, _double TimeDelta);
	void Invalidate_Camera(class CCamera* pCamera, class CTransform* pPlayerTransform, _double dTimeDelta);
	HRESULT SetUp_AnimationNotifies(vector<NOTIFY> Notifies);
	ANIMATIONFLAG Lerp_TransformMatrix(CModel::BONES& Bones, class CAnimation* pCurrentAnimation, _double TimeDelta, _double dDuration = 0.2);

public:
	_tchar						m_szName[MAX_STR] = TEXT("");
	// 애니메이션이 사용하는 채널(뼈)의 개수
	_uint						m_iNumChannels = { 0 };
	// 채널을 담고있는 벡터 컨테이너
	vector<class CChannel*>		m_Channels;
	// 각 채널의 현재 키프레임 인덱스
	vector<_uint>				m_ChannelCurrentKeyFrames;

	_double						m_dDuration = { 0.2 };
	_double						m_dOriginTickPerSecond = { 0.0 };
	_double						m_dTickPerSecond = { 0.0 };
	_double						m_dTimeAcc = { 0.0 };

	_bool						m_isLoop = { false };
	_bool						m_isPaused = { false };

	// 현재 애니메이션에 해당하는 채널의 프레임 중 가장 프레임이 많은 채널의 인덱스
	_uint						m_iMaxFrameChannelIndex = { 0 };
	// 애니메이션의 최대 프레임 개수
	_uint						m_iAnimationFrames = { 0 };
	// 현재 애니메이션에 해당하는 채널의 프레임 중 가장 큰 프레임을 기준으로 사이즈를 처리.
	// 각 프레임 별로 Notify를 관리
	vector<NOTIFY>				m_AnimationNotify;

public:
	static CAnimation* Create(Engine::ANIMATION* pAnimation, const CModel::BONES& Bones);
	CAnimation* Clone();
	virtual void Free() override;
};

END