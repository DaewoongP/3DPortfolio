#pragma once
#include "ImWindow.h"
#include "Terrain.h"
#include "Camera_Free.h"
#include "Axis.h"

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
	// �ͷ��� ���� ������
	array<_int, 2>					m_TerrainSize;
	// ���������� �ͷ��� �ִ� ������
	array<_int, 2>					m_TerrainOverflowSize;

	_bool							m_bIsWireFrame = { false };
	_float							m_fFreeCamSpeed = { 50.f };
	// ȭ�� �߾ӿ� ������ Axis�� ī�޶���� �Ÿ�
	_float							m_fAxisDistance = { 30.f };

	array<_bool, CAxis::AXIS_END>	m_AxisState;

	_int							m_iTextureIndex = { 0 };

	_bool							m_bTerrainRender = { true };

private:
	CCamera_Free*					m_pCamera_Free = { nullptr };
	CTerrain*						m_pTerrain = { nullptr };
	CAxis*							m_pAxisUI = { nullptr };
	CAxis*							m_pAxisOrigin = { nullptr };
	CAxis*							m_pAxisCenter = { nullptr };

private:
	void TerrainSizeXZ();
	void TerrainHeightMap();
	void TerrainTexture();
	void WireFrame();
	void CamSpeedAndAxisDist();
	void AxisRendering();

	void TerrainRendering();

public:
	static CWindow_Tool* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END