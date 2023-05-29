#pragma once
#include "Base.h"
#include "DummyTerrain.h"
#include "Camera_Free.h"
#include "Axis.h"
#include "DummyObject.h"

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
	CDummyTerrain*			m_pTerrain = { nullptr };
	CCamera_Free*		m_pFreeCam = { nullptr };
	CAxis*				m_pAxis = { nullptr };
	CDummyObject*		m_pDummyObject = { nullptr };

public:
	CMainFrame*			m_pMainFrm = { nullptr };
	CToolView*			m_pView = { nullptr };

public:
	virtual void Free() override;
};

END