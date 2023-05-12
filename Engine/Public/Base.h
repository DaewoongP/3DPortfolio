#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
protected:
	CBase();
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