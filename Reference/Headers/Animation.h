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
	void Set_FrameSpeed(_uint iFrameIndex, _float fSpeed);
	void Set_Pause(_bool bIsPause) { m_bIsPaused = bIsPause; }
	void Set_TickPerSecond(_double dMultiply) 
	{
		if (0 >= dMultiply)
			return;
		m_dTickPerSecond = m_dOriginTickPerSecond * dMultiply;
	}
	_uint Get_MaxKeyFrameInAnimationChannels();
	_uint Get_CurrentMaxChannelKeyFrameIndex();

public:
	HRESULT Initialize(Engine::ANIMATION* pAnimation, const CModel::BONES& Bones);
	void Invalidate_TransformationMatrix(CModel::BONES& Bones, _double TimeDelta);

public:
	_tchar						m_szName[MAX_STR] = TEXT("");
	_uint						m_iNumChannels = { 0 };
	vector<class CChannel*>		m_Channels;
	vector<_uint>				m_ChannelCurrentKeyFrames;
	_double						m_dDuration = { 0.0 };
	_double						m_dOriginTickPerSecond = { 0.0 };
	_double						m_dTickPerSecond = { 0.0 };
	_double						m_dTimeAcc = { 0.0 };

	_bool						m_bIsLoop = { false };
	_bool						m_bIsPaused = { false };
	_uint						m_iMaxNumKeyFrameChannelIndex = { 0 };
	_uint						m_iMaxNumKeyFrames;
	vector<_float>				m_FrameSpeeds;

public:
	static CAnimation* Create(Engine::ANIMATION* pAnimation, const CModel::BONES& Bones);
	CAnimation* Clone();
	virtual void Free() override;
};

END