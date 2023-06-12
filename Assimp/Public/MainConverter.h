#pragma once
#include "../Public/Model.h"

BEGIN(Converter)

class CMainConverter final : public CBase
{
private:
	explicit CMainConverter();
	virtual ~CMainConverter() = default;

public:
	HRESULT Initialize();
	HRESULT Convert();

private:
	Converter::CModel* m_pModel = { nullptr };

public:
	static CMainConverter* Create();
	virtual void Free() override;
};

END