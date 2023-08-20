#include "..\Public\MiniIconParticle.h"
#include "GameInstance.h"

CMiniIconParticle::CMiniIconParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CMiniIconParticle::CMiniIconParticle(const CMiniIconParticle& rhs)
	: CGameObject(rhs)
{
}

HRESULT CMiniIconParticle::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMiniIconParticle::Initialize(void* pArg)
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

void CMiniIconParticle::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	m_isRender = false;

	vector<_float4x4>		ParticleMatrices;

	for (auto& Particle : m_Particles)
	{
		Particle.dAge += dTimeDelta;

		if (Particle.dAge > Particle.dLifeTime)
			Particle.isAlive = false;

		if (false == Particle.isAlive)
		{
			XMStoreFloat4x4(&Particle.WorldMatrix, XMMatrixIdentity());
			ParticleMatrices.push_back(Particle.WorldMatrix);
			continue;
		}

		_float4 vPos;
		memcpy(&vPos, Particle.WorldMatrix.m[3], sizeof(_float4));
		_vector vVelocity = XMLoadFloat4(&Particle.vVelocity);
		_vector vAccel = XMLoadFloat4(&Particle.vAccel);

		vVelocity += vAccel;
		XMStoreFloat4(&Particle.vVelocity, vVelocity);

		XMStoreFloat4(&vPos, XMLoadFloat4(&vPos) + vVelocity * _float(dTimeDelta));

		XMStoreFloat4x4(&Particle.WorldMatrix, XMMatrixTranslation(vPos.x, vPos.y, vPos.z));

		ParticleMatrices.push_back(Particle.WorldMatrix);

		m_isRender = true;
	}

	m_pVIBufferCom->Tick(ParticleMatrices.data());
}

GAMEEVENT CMiniIconParticle::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (nullptr != m_pRendererCom &&
		true == m_isRender)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return GAME_NOEVENT;
}

HRESULT CMiniIconParticle::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

void CMiniIconParticle::Reset_Effect()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	for (auto& Particle : m_Particles)
	{
		Particle.dAge = { 0.0 };
		Particle.dLifeTime = { 1.0 };
		Particle.isAlive = true;
		XMStoreFloat4(&Particle.vVelocity, pGameInstance->Get_RandomVectorInSphere(2.f));
		Particle.vAccel = _float4(
			Particle.vVelocity.x / (_float)Particle.dLifeTime * -1.f,
			Particle.vVelocity.y / (_float)Particle.dLifeTime * -1.f,
			Particle.vVelocity.z / (_float)Particle.dLifeTime * -1.f,
			0.f);

		XMStoreFloat4x4(&Particle.WorldMatrix, XMMatrixIdentity());
	}

	Safe_Release(pGameInstance);
}

void CMiniIconParticle::Render_Effect(_fvector vEffectPos)
{
	if (true == m_isRender)
		return;
	Reset_Effect();

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vEffectPos);
}

HRESULT CMiniIconParticle::Add_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	m_iParticleNum = { 10 };
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Icon_Mini"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMiniIconParticle::SetUp_ShaderResources()
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

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	return S_OK;
}

CMiniIconParticle* CMiniIconParticle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMiniIconParticle* pInstance = new CMiniIconParticle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CMiniIconParticle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMiniIconParticle::Clone(void* pArg)
{
	CMiniIconParticle* pInstance = new CMiniIconParticle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CMiniIconParticle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMiniIconParticle::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}
