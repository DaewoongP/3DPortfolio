#pragma once
#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	explicit CAnimation();
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(const aiAnimation* pAIAnimation);
	void Invalidate_TransformationMatrix(_double dTimeDelta);

private:
	_char						m_szName[MAX_PATH] = "";
	_uint						m_iNumChannels = { 0 };
	vector<class CChannel*>		m_Channels;
	_double						m_Duration = { 0.0 };
	_double						m_TickPerSecond = { 0.0 };

	_double						m_TimeAcc = { 0.0 };

public:
	static CAnimation* Create(const aiAnimation* pAIAnimation);
	virtual void Free() override;
};

END