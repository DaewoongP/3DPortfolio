#pragma once
#include "../Public/ModelConverter.h"

BEGIN(Converter)

class CMainConverter final : public CBase
{
private:
	explicit CMainConverter();
	virtual ~CMainConverter() = default;

public:
	// ���� ���� �Լ�
	HRESULT Convert();

private:
	// ���丮�� ��ȸ�ϸ鼭 �������� ���̳ʸ�ȭ �ϸ� Ÿ�� ����.
	// 1. eType : �ִϸ��̼Ǹ�, ����ƽ�� Ÿ�� ����
	// 2. DirectoryPath : ��ȸ�� �ֻ������� ����.
	HRESULT ReadFileInDirectory(CModelConverter::TYPE eType, const _tchar* pDirectoryPath);

public:
	// ���� Ŭ�������� ������ �Լ�
	static void Start();
	virtual void Free() override;
};

END