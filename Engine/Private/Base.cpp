#include "..\Public\Base.h"

CBase::CBase()
{
}

_ulong CBase::AddRef()
{
	/* ������Ű�� ������ ���� �����Ѵ�. */
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
	// ��ġ�����ϰ�� 0���� ���� �ɶ��� 1���� �����ɶ��� 
	// ���� ���� �ϹǷ� ����׻󿡼� �̵��� ��������
	// ������ ���� �����Ͽ� �Ǵ�.
	// 0�ϰ�� ��ü�� ���� �����ȰŰ�, 1�ϰ�� ������ ���� ���� ��ü�� �����ִ°��.
	else
		return m_dwRefCnt--;
}

