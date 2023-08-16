#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Point_Color_Instance;
END

BEGIN(Client)

class CBloodParticle final : public CGameObject
{
public:
	typedef struct tagParticle
	{
		_bool		isAlive;
		_float4		vColor;
		_float4		vAccel;
		_float4		vVelocity;
		_float4x4	WorldMatrix;
		_float		fAngle;
		_double		dAge;
		_double		dLifeTime;
	}PARTICLE;

private:
	explicit CBloodParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBloodParticle(const CBloodParticle& rhs);
	virtual ~CBloodParticle() = default;

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual GAMEEVENT Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	void Reset_Effect();
	void Reset_Particle(PARTICLE & Particle);
	void Render_Effect(_fvector vEffectPos);

private:
	CShader*							m_pShaderCom = { nullptr };
	CTexture*							m_pTextureCom = { nullptr };
	CRenderer*							m_pRendererCom = { nullptr };
	CVIBuffer_Point_Color_Instance*		m_pVIBufferCom = { nullptr };

private:
	_uint								m_iParticleNum = { 0 };
	vector<PARTICLE>					m_Particles;

private:
	_double								m_dGenTime = { 0.0 };

public:
	static CBloodParticle* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END