#pragma once
#include "Tool_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Terrain;
END

BEGIN(Tool)
class CToolInstance;

class CTerrain final : public CGameObject
{
private:
	explicit CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTerrain(const CTerrain& rhs);
	virtual ~CTerrain() = default;

public:
	void Set_WireFrame(_bool isWireFrame) { m_bIsWireFrame = isWireFrame; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT RemakeTerrain(_uint iSizeX, _uint iSizeY);

private:
	CShader*				m_pShaderCom = { nullptr };
	CRenderer*				m_pRendererCom = { nullptr };
	CVIBuffer_Terrain*		m_pTerrainCom = { nullptr };

	_bool					m_bIsWireFrame = { true };

private:
	CToolInstance*			m_pToolInstance = { nullptr };

private:
	HRESULT		Add_Components();
	HRESULT		SetUp_ShaderResources();

public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END