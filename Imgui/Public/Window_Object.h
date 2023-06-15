#pragma once
#include "ImWindow.h"

BEGIN(Tool)

class CWindow_Object : public CImWindow
{
private:
	explicit CWindow_Object();
	virtual ~CWindow_Object() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;

public:
	static CWindow_Object* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END