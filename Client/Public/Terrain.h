#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CNavigation;
class CVIBuffer_Terrain;
END

BEGIN(Client)

class CTerrain : public CGameObject
{
private:
	explicit CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTerrain(const CTerrain& rhs);
	virtual ~CTerrain() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual void Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Component();
	HRESULT SetUp_ShaderResources();

private:
	CShader*			m_pShaderCom = { nullptr };
	CTexture*			m_pTextureCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CTransform*			m_pTransformCom = { nullptr };
	CNavigation*		m_pNavigationCom = { nullptr };
	CVIBuffer_Terrain*	m_pBufferCom = { nullptr };

public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END