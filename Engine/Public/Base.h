#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
protected:
	explicit CBase();
	virtual ~CBase() = default;

public:
	_ulong AddRef(); 
	_ulong Release();

protected:
	_ulong			m_dwRefCnt = { 1 };

public:
	virtual void Free() PURE;
};

END