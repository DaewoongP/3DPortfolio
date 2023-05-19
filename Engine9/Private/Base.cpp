#include "Base.h"
/// <summary>
/// Engine9 Base
/// </summary>
Engine9::CBase::CBase()
{
}

Engine9::_ulong Engine9::CBase::AddRef()
{
	return ++m_dwRefCnt;
}

Engine9::_ulong Engine9::CBase::Release()
{
	if (0 == m_dwRefCnt)
	{
		Free();

		delete this;

		return 0;
	}
	else
		return m_dwRefCnt--;
}