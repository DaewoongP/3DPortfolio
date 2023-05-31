#pragma once
#include "ImWindow.h"
#include "Terrain.h"
#include "Camera_Free.h"

BEGIN(Tool)

class CWindow_Tool : public CImWindow
{
private:
	explicit CWindow_Tool();
	virtual ~CWindow_Tool() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;

private:
	array<_int, 2>				m_TerrainSize;
	array<_int, 2>				m_TerrainOverflowSize;
	_bool						m_bIsWireFrame = { false };
	_float						m_fFreeCamSpeed = { 5.f };

	CCamera_Free*				m_pCamera_Free = { nullptr };
	CTerrain*					m_pTerrain = { nullptr };

public:
	static CWindow_Tool* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END