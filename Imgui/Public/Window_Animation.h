#pragma once
#include "ImWindow.h"


BEGIN(Tool)

class CWindow_Animation : public CImWindow
{
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
	_double						m_dAnimationSpeed = { 1.0 };

private:
	HRESULT AnimationIndex();
	HRESULT AnimationSpeed();

public:
	static CWindow_Animation* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END