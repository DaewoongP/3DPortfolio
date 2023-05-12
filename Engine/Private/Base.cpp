#include "..\Public\Base.h"

CBase::CBase()
{
}

_ulong CBase::AddRef()
{
	/* 증가시키고난 이후의 값을 리턴한다. */
	return ++m_dwRefCnt;
}

_ulong CBase::Release()
{
	if (0 == m_dwRefCnt)
	{
		Free();

		delete this;

		return 0;
	}
	// 전치연산일경우 0에서 삭제 될때와 1에서 삭제될때의 
	// 값이 동일 하므로 디버그상에서 이득을 보기위해
	// 감소전 값을 리턴하여 판단.
	// 0일경우 객체가 실제 삭제된거고, 1일경우 참조가 없는 실제 객체만 남아있는경우.
	else
		return m_dwRefCnt--;
}

