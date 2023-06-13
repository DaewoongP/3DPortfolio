#pragma once
#include "../Public/Model.h"

BEGIN(Converter)

class CMainConverter final : public CBase
{
private:
	explicit CMainConverter();
	virtual ~CMainConverter() = default;

public:
	HRESULT Convert();

private:
	HRESULT ReadFileInDirectory(CModel::TYPE eType, const _tchar* pDirectoryPath);

private:
	vector<const _tchar*>		m_Files;

public:
	static void Start();
	virtual void Free() override;
};

END