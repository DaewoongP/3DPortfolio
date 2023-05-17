#pragma once
#include "Base.h"
#include "Tool_Defines.h"

class CToolView;
class CMainFrame;

BEGIN(Engine9)
class CGameInstance9;
class CRenderer;
END

BEGIN(Tool)

class CMainTool final : public CBase
{
private:
	explicit CMainTool();
	virtual ~CMainTool() = default;

public:
	HRESULT Initialize();
	void Tick(_double dTimeDelta);
	void Render(void);

private:
	HRESULT Ready_Prototype_Component();
	HRESULT	Ready_Prototype_Object();

private:
	CMainFrame*					m_pMainFrm = { nullptr };
	CToolView*					m_pView = { nullptr };
	LPDIRECT3DDEVICE9			m_pDevice = { nullptr };
	
private:
	CGameInstance9*	m_pGameInstance = { nullptr };
	CRenderer*		m_pRenderer = { nullptr };

public:
	static CMainTool* Create();
	virtual void Free() override;
};

END