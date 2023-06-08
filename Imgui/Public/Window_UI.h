#pragma once
#include "ImWindow.h"

BEGIN(Tool)

class CWindow_UI final : public CImWindow
{
private:
	explicit CWindow_UI();
	virtual ~CWindow_UI() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;

private:
	vector<class CGameObject*>			m_Undo;
	_uint								m_iMaxSelection = { 10 };
public:
	static CWindow_UI* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END