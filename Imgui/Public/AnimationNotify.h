#pragma once
#include "Base.h"
#include "Tool_Defines.h"

BEGIN(Tool)

class CAnimationNotify final : public CBase
{
private:
	explicit CAnimationNotify();
	virtual ~CAnimationNotify() = default;

public:
	HRESULT Initialize(void* pArg);
	void Tick(_double dTimeDelta);

private:
	HRESULT NotifyCamera();

public:
	static CAnimationNotify* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END