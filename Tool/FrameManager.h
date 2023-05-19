#pragma once
#include "Base.h"

class CFrameManager final : public CBase
{
	DECLARE_SINGLETON(CFrameManager)
private:
	explicit CFrameManager();
	virtual ~CFrameManager() = default;


public:
	virtual void Free() override;
};

