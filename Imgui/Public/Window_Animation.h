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
		static _float FrameSpeedSaw(void* pArg, _int i) 
		{
			vector<_float> FrameSpeed;
			FrameSpeed = *reinterpret_cast<vector<_float>*>(pArg);
			if (0 == FrameSpeed.size())
				return 0;
			return FrameSpeed[i];
		}
	};

private:
	explicit CWindow_Animation();
	virtual ~CWindow_Animation() = default;

public:
	vector<_float> Get_FrameSpeeds() const { return m_FrameSpeeds; }
	void Set_CurrentAnimModel(class CAnimModel* pAnimModel);

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;

private:
	class CAnimModel*			m_pCurrentAnimModel = { nullptr };
	_char						m_szAnimationName[MAX_STR] = "";
	_int						m_iAnimationIndex = { 0 };
	_uint						m_iAnimationMaxKeyFrames = { 0 };
	_uint						m_iCurrentAnimationFrameIndex = { 0 };
	_double						m_dAnimationSpeed = { 1.0 };
	_bool						m_bPauseButton = { false };

	vector<_float>				m_FrameSpeeds;
	_float						(*m_FrameSpeedFunc)(void*, _int);

private:
	HRESULT AnimationIndex();
	HRESULT AnimationPause();
	HRESULT AnimationSpeed();

public:
	static CWindow_Animation* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END