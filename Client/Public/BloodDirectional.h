#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Point_Instance;
END

BEGIN(Client)

class CBloodDirectional final : public CGameObject
{
private:
	explicit CBloodDirectional(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBloodDirectional(const CBloodDirectional& rhs);
	virtual ~CBloodDirectional() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual GAMEEVENT Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Render_Effect(_double dRenderTime, _fvector vEffectPos);

private:
	CShader*						m_pShaderCom = { nullptr };
	CTexture*						m_pTextureCom = { nullptr };
	CRenderer*						m_pRendererCom = { nullptr };
	CVIBuffer_Point_Instance*		m_pVIBufferCom = { nullptr };

private:
	_uint							m_iParticleNum = { 0 };

private:
	_double							m_dRenderTime = { 0.0 };
	_double							m_dRenderTimeAcc = { 0.0 };

public:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CBloodDirectional* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END