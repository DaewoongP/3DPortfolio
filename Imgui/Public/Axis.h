#pragma once
#include "Tool_Defines.h"
#include "GameObject.h"
#include "GameInstance.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Line;
END

BEGIN(Tool)

class CAxis final : public CGameObject
{
public:
	enum AXISSTATE { AXIS_ORIGIN, AXIS_UI, AXIS_CENTER, AXIS_END };

private:
	explicit CAxis(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CAxis(const CAxis& rhs);
	virtual ~CAxis() = default;

public:
	void Set_AxisDistance(_float fDistance) { m_fDistance = fDistance; }
	void Set_AxisOriginScale(_float fScale) { XMStoreFloat3(&m_vOriginAxisScale, XMLoadFloat3(&m_vOriginAxisScale) * fScale); }
	void Set_Rendering(_bool isRender) { m_isRendering = isRender; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual void Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer*				m_pRendererCom = { nullptr };
	CVIBuffer_Line*			m_pLineCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };

private:
	_float		m_fDistance = { 0 };
	_float3		m_vOriginAxisScale;
	// 현재 Axis가 UI인지, 원점인지, 중앙인지 판단
	AXISSTATE	m_eState;

	_float			m_fX = { 0.f };
	_float			m_fY = { 0.f };
	_float			m_fUIAxisSize = { 0.f };

	_bool			m_isRendering = { true };

	_float4x4		m_WorldMatrix;
	_float4x4		m_ViewMatrix;
	_float4x4		m_ProjMatrix;

private:
	HRESULT		Add_Component();
	HRESULT		SetUp_ShaderResources();
	void		Set_Position();

	void Set_Origin(CGameInstance* pGameInstance);
	void Set_UI(CGameInstance* pGameInstance);
	void Set_Center(CGameInstance* pGameInstance);

public:
	static CAxis* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END