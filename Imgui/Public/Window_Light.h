#pragma once

#include "ImWindow.h"

BEGIN(Tool)

class CWindow_Light final : public CImWindow
{
private:
	explicit CWindow_Light();
	virtual ~CWindow_Light() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;

private:
	void MakeObject();

public:
	static CWindow_Light* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END;