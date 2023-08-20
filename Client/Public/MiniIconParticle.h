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

class CMiniIconParticle final : public CGameObject
{
public:
	typedef struct tagParticle
	{
		_bool		isAlive;
		_float4		vAccel;
		_float4		vVelocity;
		_float4x4	WorldMatrix;
		_double		dAge;
		_double		dLifeTime;
	}PARTICLE;

private:
	explicit CMiniIconParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMiniIconParticle(const CMiniIconParticle& rhs);
	virtual ~CMiniIconParticle() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual GAMEEVENT Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Reset_Effect();
	void Render_Effect(_fvector vEffectPos);

private:
	CShader*					m_pShaderCom = { nullptr };
	CTexture*					m_pTextureCom = { nullptr };
	CRenderer*					m_pRendererCom = { nullptr };
	CVIBuffer_Point_Instance*	m_pVIBufferCom = { nullptr };

private:
	_uint						m_iParticleNum = { 0 };
	vector<PARTICLE>			m_Particles;

private:
	_bool						m_isRender = { false };

public:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CMiniIconParticle* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END