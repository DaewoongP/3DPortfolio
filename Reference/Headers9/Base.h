#pragma once

#include "Engine9_Defines.h"

BEGIN(Engine9)

class ENGINE_DLL CBase abstract
{
protected:
	explicit CBase();
	virtual ~CBase() = default;

public:
	_ulong AddRef(); 
	_ulong Release();

protected:
	_ulong			m_dwRefCnt = { 0 };

public:
	virtual void Free() PURE;
};

END