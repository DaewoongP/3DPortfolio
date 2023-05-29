#pragma once
#include "ImWindow.h"

BEGIN(Tool)

class CWindow_Terrain : public CImWindow
{
private:
	explicit CWindow_Terrain();
	virtual ~CWindow_Terrain() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;

public:
	static CWindow_Terrain* Create(void* pArg = nullptr);

};

END