#pragma once
#include "ImWindow.h"

BEGIN(Tool)

class CWindow_Camera : public CImWindow
{
private:
	explicit CWindow_Camera();
	virtual ~CWindow_Camera() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;

public:
	static CWindow_Camera* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END