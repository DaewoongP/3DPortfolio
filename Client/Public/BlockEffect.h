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

class CBlockEffect final : public CGameObject
{
public:
	typedef struct tagBlockParticle
	{
		// ��ƼŬ�� ����ִ���
		_bool		isAlive;
		// ���ӵ�
		_float4		vAccel;
		// �ӵ�
		_float4		vVelocity;
		// ��ƼŬ ��ü�� �������
		_float4x4	WorldMatrix;
		// ��ƼŬ�� ����
		_double		dAge;
		// ��ƼŬ�� ������Ÿ��
		_double		dLifeTime;
	}BLOCKPARTICLE;

private:
	explicit CBlockEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBlockEffect(const CBlockEffect& rhs);
	virtual ~CBlockEffect() = default;

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
	vector<BLOCKPARTICLE>		m_Particles;

private:
	_bool						m_isRender = { false };

public:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CBlockEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END