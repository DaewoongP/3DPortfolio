#pragma once
#include "Tool_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Terrain;
END

BEGIN(Tool)

class CTerrain : public CGameObject
{
private:
	explicit CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTerrain(const CTerrain& rhs);
	virtual ~CTerrain() = default;

public:
	void Set_WireFrame(_bool isWireFrame) { m_bIsWireFrame = isWireFrame; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual void Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT RemakeTerrain(_uint iSizeX, _uint iSizeY);

private:
	CShader*				m_pShaderCom = { nullptr };
	CTexture*				m_pTextureCom = { nullptr };
	CRenderer*				m_pRendererCom = { nullptr };
	CVIBuffer_Terrain*		m_pTerrainCom = { nullptr };

	_bool					m_bIsWireFrame = { false };

private:
	HRESULT		Add_Component();
	HRESULT		SetUp_ShaderResources();

public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END