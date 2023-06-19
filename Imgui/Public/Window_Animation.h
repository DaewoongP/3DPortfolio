#pragma once
#include "ImWindow.h"


BEGIN(Tool)

class CWindow_Animation : public CImWindow
{
public:
	struct Funcs // graph style
	{
		static float Sin(void*, int i) { return sinf(i * 0.1f); }
		static float Saw(void*, int i) { return (i & 1) ? 1.0f : -1.0f; }
	};

private:
	explicit CWindow_Animation();
	virtual ~CWindow_Animation() = default;

public:
	void Set_CurrentAnimModel(class CAnimModel* pAnimModel);

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;

private:
	class CAnimModel*			m_pCurrentAnimModel = { nullptr };
	_int						m_iAnimationIndex = { 0 };
	_uint						m_iAnimationKeyFrames = { 0 };
	_uint						m_iCurrentAnimationFrameIndex = { 0 };
	_double						m_dAnimationSpeed = { 1.0 };
	_bool						m_bPauseButton = { false };

	vector<_float>				m_FrameSpeeds;

private:
	HRESULT AnimationIndex();
	HRESULT AnimationSpeed();

public:
	static CWindow_Animation* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END