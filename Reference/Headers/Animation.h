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
	HRESULT Initialize(Engine::ANIMATION* pAnimation, const CModel::BONES& Bones);
	void Invalidate_TransformationMatrix(CModel::BONES& Bones, _double TimeDelta);

public:
	_tchar						m_szName[MAX_STR] = TEXT("");
	_uint						m_iNumChannels = { 0 };
	vector<class CChannel*>		m_Channels;
	vector<_uint>				m_ChannelCurrentKeyFrames;
	_double						m_dDuration = { 0.0 };
	_double						m_dTickPerSecond = { 0.0 };
	_double						m_dTimeAcc = { 0.0 };

	_bool						m_bIsLoop = { false };

public:
	static CAnimation* Create(Engine::ANIMATION* pAnimation, const CModel::BONES& Bones);
	CAnimation* Clone();
	virtual void Free() override;
};

END