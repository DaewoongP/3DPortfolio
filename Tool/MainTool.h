#pragma once
#include "Base.h"
#include "Tool_Defines.h"

class CToolView;
class CMainFrame;

BEGIN(Engine)
class CGameInstance;
class CRenderer;
END

BEGIN(Tool)
class CToolInstance;
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
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	
private:
	CGameInstance*				m_pGameInstance = { nullptr };
	CToolInstance*				m_pToolInstance = { nullptr };
	CRenderer*					m_pRenderer = { nullptr };

public:
	static CMainTool* Create();
	virtual void Free() override;
};

END