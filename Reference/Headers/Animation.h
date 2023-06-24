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
	_uint Get_AnimationFrames();
	// 현재 애니메이션의 프레임 반환
	// 채널 중 키프레임 최대치 기준.
	_uint Get_CurrentAnimationFrame();
	// 프레임 인덱스에 해당하는 스피드값 설정.
	void Set_FrameSpeed(_uint iFrameIndex, _float fSpeed);
	void Set_CurrentKeyFrameIndex(CModel::BONES& Bones, _uint iKeyFrameIndex);
	void Set_Pause(_bool isPause) { m_isPaused = isPause; }
	void Set_TickPerSecond(_double dMultiply) 
	{
		if (0 >= dMultiply)
			return;

		m_dTickPerSecond = m_dOriginTickPerSecond * dMultiply;
	}
	// 카메라 eye, at값을 프레임에 저장
	void Set_CameraValueInFrame(_uint iFrameIndex, _float3 vEye, _float3 vAt);

public:
	HRESULT Initialize(Engine::ANIMATION* pAnimation, const CModel::BONES& Bones);
	void Invalidate_TransformationMatrix(CModel::BONES& Bones, _double TimeDelta);

public:
	_tchar						m_szName[MAX_STR] = TEXT("");
	// 애니메이션이 사용하는 채널(뼈)의 개수
	_uint						m_iNumChannels = { 0 };
	// 채널을 담고있는 벡터 컨테이너
	vector<class CChannel*>		m_Channels;
	// 각 채널의 현재 키프레임 인덱스
	vector<_uint>				m_ChannelCurrentKeyFrames;

	_double						m_dDuration = { 0.0 };
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
	// 각 프레임 별로 스피드를 관리하는 변수.
	vector<_float>				m_AnimationFrameSpeeds;

public:
	static CAnimation* Create(Engine::ANIMATION* pAnimation, const CModel::BONES& Bones);
	CAnimation* Clone();
	virtual void Free() override;
};

END