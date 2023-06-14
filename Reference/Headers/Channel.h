#pragma once
#include "Model.h"

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	explicit CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(const Engine::CHANNEL& Channel, const CModel::BONES& Bones);
	void Invalidate_TransformationMatrix(CModel::BONES& Bones, _double dTimeAcc, _uint* pCurrentKeyFrameIndex);

public:
	_tchar				m_szName[MAX_STR] = TEXT("");
	_uint				m_iNumKeyFrames = { 0 };
	vector<KEYFRAME>	m_KeyFrames;
	_uint				m_iBoneIndex = { 0 };

public:
	static CChannel* Create(const Engine::CHANNEL& Channel, const CModel::BONES& Bones);
	virtual void Free() override;
};

END