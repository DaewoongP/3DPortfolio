#include "..\Public\ShurikenParticle.h"
#include "GameInstance.h"

CShurikenParticle::CShurikenParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CShurikenParticle::CShurikenParticle(const CShurikenParticle& rhs)
	: CGameObject(rhs)
{
}

HRESULT CShurikenParticle::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CShurikenParticle::Initialize(void* pArg)
{
	CTransform::TRANSFORMDESC TransformDesc;
	ZEROMEM(&TransformDesc);

	if (FAILED(__super::Initialize(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Reset_Effect();

	return S_OK;
}

void CShurikenParticle::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	vector<_float4x4>		ParticleMatrices;

	for (auto& Particle : m_Particles)
	{
		Particle.dAge += dTimeDelta;

		if (Particle.dAge < Particle.dGenTime)
		{
			Reset_Particle(Particle);
			ParticleMatrices.push_back(Particle.WorldMatrix);
			continue;
		}

		if (Particle.dAge > Particle.dLifeTime + Particle.dGenTime)
			Particle.isAlive = false;

		if (false == Particle.isAlive)
		{
			Reset_Particle(Particle);
		}

		_float4 vPos;
		memcpy(&vPos, Particle.WorldMatrix.m[3], sizeof(_float4));
		_vector vVelocity = XMLoadFloat4(&Particle.vVelocity);
		_vector vAccel = XMLoadFloat4(&Particle.vAccel);

		vVelocity += vAccel * _float(dTimeDelta);
		XMStoreFloat4(&Particle.vVelocity, vVelocity);

		XMStoreFloat4(&vPos, XMLoadFloat4(&vPos) + vVelocity * _float(dTimeDelta));
		
		XMStoreFloat4x4(&Particle.WorldMatrix, XMMatrixScaling(0.05f, 0.05f, 0.05f)*
			XMMatrixTranslation(vPos.x, vPos.y, vPos.z));

		ParticleMatrices.push_back(Particle.WorldMatrix);
	}

	m_pVIBufferCom->Tick(ParticleMatrices.data());
}

GAMEEVENT CShurikenParticle::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);

	return GAME_NOEVENT;
}

HRESULT CShurikenParticle::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);

	m_pVIBufferCom->Render();

	return S_OK;
}

void CShurikenParticle::Reset_Effect()
{
	for (auto& Particle : m_Particles)
	{
		Reset_Particle(Particle);
	}
}

void CShurikenParticle::Reset_Particle(PARTICLE& Particle)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	Particle.dAge = { 0.0 };
	Particle.dLifeTime = { 3.0 };
	Particle.dGenTime = (rand() % 200) / 100.0;
	Particle.isAlive = true;
	XMStoreFloat4(&Particle.vVelocity, pGameInstance->Get_RandomVectorInSphere(0.2f));
	Particle.vAccel = _float4(
		Particle.vVelocity.x / (_float)Particle.dLifeTime * -1.f,
		Particle.vVelocity.y / (_float)Particle.dLifeTime * -1.f,
		Particle.vVelocity.z / (_float)Particle.dLifeTime * -1.f,
		0.f);

	XMStoreFloat4x4(&Particle.WorldMatrix, XMMatrixScaling(0.05f, 0.05f, 0.05f));

	Safe_Release(pGameInstance);
}

void CShurikenParticle::Render_Effect(_fvector vEffectPos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vEffectPos);
}

HRESULT CShurikenParticle::Add_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	m_iParticleNum = { 30 };
	m_Particles.resize(m_iParticleNum);

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point_Instance"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom, &m_iParticleNum)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Circle_Particle"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CShurikenParticle::SetUp_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	if (FAILED(m_pTextureCom->Bind_ShaderResources(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	_float4 vColor = _float4(0.062f, 0.988f, 0.99f, 1.f);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

CShurikenParticle* CShurikenParticle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CShurikenParticle* pInstance = new CShurikenParticle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CShurikenParticle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CShurikenParticle::Clone(void* pArg)
{
	CShurikenParticle* pInstance = new CShurikenParticle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CShurikenParticle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShurikenParticle::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}
