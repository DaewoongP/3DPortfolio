#pragma once
#include "Base.h"
#include "Terrain.h"
#include "DynamicCamera.h"
#include "Axis.h"

class CToolView;
class CMainFrame;

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Tool)

class CToolInstance final : public CBase
{
	DECLARE_SINGLETON(CToolInstance)
private:
	explicit CToolInstance();
	virtual ~CToolInstance() = default;

public:
	CTerrain*			m_pTerrain = { nullptr };
	CDynamicCamera*		m_pDynamicCamera = { nullptr };
	CAxis*				m_pAxis = { nullptr };

public:
	CMainFrame*			m_pMainFrm = { nullptr };
	CToolView*			m_pView = { nullptr };

public:
	virtual void Free() override;
};

END