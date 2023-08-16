#include "..\Public\BloodParticle.h"
#include "GameInstance.h"

CBloodParticle::CBloodParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CBloodParticle::CBloodParticle(const CBloodParticle& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBloodParticle::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBloodParticle::Initialize(void* pArg)
{
	CTransform::TRANSFORMDESC TransformDesc;
	ZEROMEM(&TransformDesc);

	if (FAILED(__super::Initialize(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;
	for (auto& Particle : m_Particles)
		ZEROMEM(&Particle);

	return S_OK;
}

void CBloodParticle::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	vector<CVIBuffer_Point_Color_Instance::COLORINSTANCE>		ParticleInstances;

	for (auto& Particle : m_Particles)
	{
		Particle.dAge += dTimeDelta;

		if (Particle.dAge > Particle.dLifeTime)
			Particle.isAlive = false;

		Particle.vColor.w = 1.f - _float(Particle.dAge / Particle.dLifeTime);

		_float4 vPos;
		memcpy(&vPos, Particle.WorldMatrix.m[3], sizeof(_float4));
		_vector vVelocity = XMLoadFloat4(&Particle.vVelocity);
		_vector vAccel = XMLoadFloat4(&Particle.vAccel);

		vVelocity += vAccel * _float(dTimeDelta);
		XMStoreFloat4(&Particle.vVelocity, vVelocity);

		XMStoreFloat4(&vPos, XMLoadFloat4(&vPos) + vVelocity * _float(dTimeDelta));

		_float fScale;
		fScale = 0.5f;

		XMStoreFloat4x4(&Particle.WorldMatrix, XMMatrixScaling(fScale, fScale, fScale) *
			XMMatrixTranslation(vPos.x, vPos.y, vPos.z));

		CVIBuffer_Point_Color_Instance::COLORINSTANCE ColorInstance;
		ColorInstance.InstanceLocalMatrix = Particle.WorldMatrix;
		ColorInstance.vInstanceColor = Particle.vColor;
		ParticleInstances.push_back(ColorInstance);
	}

	m_pVIBufferCom->Tick(ParticleInstances.data(), true, m_pTransformCom->Get_WorldMatrix_Inverse());
}

GAMEEVENT CBloodParticle::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);

	return GAME_NOEVENT;
}

HRESULT CBloodParticle::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CBloodParticle::Add_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	m_iParticleNum = { 50 };
	m_Particles.resize(m_iParticleNum);

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point_Color_Instance"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom, &m_iParticleNum)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPointColorInstance"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Circle_Particle"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBloodParticle::SetUp_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	if (FAILED(m_pTextureCom->Bind_ShaderResources(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	return S_OK;
}

void CBloodParticle::Reset_Effect()
{
	for (auto& Particle : m_Particles)
	{
		Reset_Particle(Particle);
	}
}

void CBloodParticle::Reset_Particle(PARTICLE& Particle)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	Particle.vColor = _float4(1.f, 0.f, 0.f, 1.f);
	Particle.dAge = { 0.0 };
	Particle.dLifeTime = { 2.0 };
	Particle.isAlive = true;
	XMStoreFloat4(&Particle.vVelocity, pGameInstance->Get_RandomVectorInSphere(10.f));
	Particle.vAccel = _float4(0.f, -20.f, 0.f, 0.f);

	XMStoreFloat4x4(&Particle.WorldMatrix, XMMatrixScaling(0.5f, 0.5f, 0.5f));

	Safe_Release(pGameInstance);
}

void CBloodParticle::Render_Effect(_fvector vEffectPos)
{
	Reset_Effect();

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vEffectPos);
}

CBloodParticle* CBloodParticle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBloodParticle* pInstance = new CBloodParticle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBloodParticle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBloodParticle::Clone(void* pArg)
{
	CBloodParticle* pInstance = new CBloodParticle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CBloodParticle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBloodParticle::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}
