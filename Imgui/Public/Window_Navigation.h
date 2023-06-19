#pragma once
#include "ImWindow.h"

BEGIN(Tool)

class CWindow_Navigation : public CImWindow
{
private:
	explicit CWindow_Navigation();
	virtual ~CWindow_Navigation() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;

public:
	static CWindow_Navigation* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END