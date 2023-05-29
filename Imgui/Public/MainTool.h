#pragma once
#include "Base.h"
#include "GameInstance.h"
#include "ImWindow_Manager.h"
#include "Tool_Defines.h"
#include "Engine_Defines.h"

BEGIN(Engine)
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
	HRESULT Render();

public:
	HRESULT Ready_Prototype_Component();
	HRESULT Ready_Prototype_Object();
	HRESULT Open_Level();

private:
	void Render_FPS(_double dTimeDelta);

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	ImGuiIO*				m_pIO = { nullptr };

private:
	CImWindow_Manager*		m_pImWindow_Manager = { nullptr };
	CGameInstance*			m_pGameInstance = { nullptr };
	CRenderer*				m_pRenderer = { nullptr };

private:
	TCHAR					m_szFPS[MAX_STR];
	_int					m_iFps = { 0 };
	_double					m_dFpsTime = { 0.0 };

public:
	static CMainTool* Create();
	virtual void Free() override;
};

END