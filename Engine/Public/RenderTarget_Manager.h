#pragma once
#include "Base.h"

BEGIN(Engine)

class CRenderTarget_Manager final : public CBase
{
	DECLARE_SINGLETON(CRenderTarget_Manager)
private:
	explicit CRenderTarget_Manager();
	virtual ~CRenderTarget_Manager() = default;

private:
	unordered_map<const _tchar*, class CRenderTarget*>		m_RenderTargets;

public:	
	virtual void Free() override;
};

END