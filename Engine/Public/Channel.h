#pragma once
#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	explicit CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(const aiNodeAnim* pAIChannel);
	void Invalidate_TransformationMatrix(_double dTimeAcc);

private:
	_char				m_szName[MAX_PATH] = "";
	_uint				m_iNumKeyFrames = { 0 };
	vector<KEYFRAME>	m_KeyFrames;
	_uint				m_iCurrentKeyFrame = { 0 };

public:
	static CChannel* Create(const aiNodeAnim* pAIChannel);
	virtual void Free() override;
};

END