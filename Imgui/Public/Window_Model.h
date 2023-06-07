#pragma once
#include "ImWindow.h"
#include "Terrain.h"

BEGIN(Tool)

class CWindow_Model : public CImWindow
{
private:
	explicit CWindow_Model();
	virtual ~CWindow_Model() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;

private:
	CTerrain*		m_pTerrain = { nullptr };

private:
	_bool			m_bPickMeshes = { false };

private:
	HRESULT	Select_MeshFiles();
	HRESULT Setting_Transform();
	HRESULT MakeObject(_double dTimeDelta);

public:
	static CWindow_Model* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END