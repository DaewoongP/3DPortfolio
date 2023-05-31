#pragma once
#include "ImWindow.h"

BEGIN(Tool)

class CWindow_UI : public CImWindow
{
private:
	explicit CWindow_UI();
	virtual ~CWindow_UI() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;

public:
	static CWindow_UI* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END