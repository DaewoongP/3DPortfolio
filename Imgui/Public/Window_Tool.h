#pragma once
#include "ImWindow.h"
#include "Terrain.h"
#include "Camera_Free.h"
#include "Axis.h"
#include "DummyObject.h"

BEGIN(Tool)

class CWindow_Tool final : public CImWindow
{
private:
	explicit CWindow_Tool();
	virtual ~CWindow_Tool() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;

private:
	array<_int, 2>					m_TerrainSize;
	array<_int, 2>					m_TerrainOverflowSize;
	_bool							m_bIsWireFrame = { false };
	_float							m_fFreeCamSpeed = { 5.f };
	_float							m_fAxisDistance = { 3.f };
	array<_bool, CAxis::AXIS_END>	m_AxisState;
	_float4							m_vPickPos;

	CCamera_Free*					m_pCamera_Free = { nullptr };
	CTerrain*						m_pTerrain = { nullptr };
	CAxis*							m_pAxisUI = { nullptr };
	CAxis*							m_pAxisOrigin = { nullptr };
	CAxis*							m_pAxisCenter = { nullptr };
	CDummyObject*					m_pDummy = { nullptr };

private:
	void TerrainSizeXZ();
	void TerrainHeightMap();
	void WireFrame();
	void CamSpeedAndAxisDist();
	void AxisRendering();

	HRESULT MakeObject(_double dTimeDelta);

public:
	static CWindow_Tool* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END