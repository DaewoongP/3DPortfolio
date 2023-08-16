#include "..\Public\ExplodeParticle.h"
#include "GameInstance.h"

CExplodeParticle::CExplodeParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CExplodeParticle::CExplodeParticle(const CExplodeParticle& rhs)
	: CGameObject(rhs)
{
}

HRESULT CExplodeParticle::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CExplodeParticle::Initialize(void* pArg)
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

void CExplodeParticle::Tick(_double dTimeDelta)
{
	if (false == m_isRender)
		return;

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

		XMStoreFloat4x4(&Particle.WorldMatrix, XMMatrixScaling(5.f, 5.f, 5.f) * XMMatrixTranslation(vPos.x, vPos.y, vPos.z));

		CVIBuffer_Point_Color_Instance::COLORINSTANCE ColorInstance;
		ColorInstance.InstanceLocalMatrix = Particle.WorldMatrix;
		ColorInstance.vInstanceColor = Particle.vColor;
		ParticleInstances.push_back(ColorInstance);
	}

	m_pVIBufferCom->Tick(ParticleInstances.data(), true, m_pTransformCom->Get_WorldMatrix_Inverse());
}

GAMEEVENT CExplodeParticle::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (nullptr != m_pRendererCom &&
		true == m_isRender)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);

	if (true == m_isRender &&
		true == IsParticleFinished())
	{
		m_isRender = false;
	}

	return GAME_NOEVENT;
}

HRESULT CExplodeParticle::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

void CExplodeParticle::Reset_Effect()
{
	for (auto& Particle : m_Particles)
	{
		Reset_Particle(Particle);
	}
}

void CExplodeParticle::Reset_Particle(PARTICLE& Particle)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	Particle.vColor = _float4(0.71875f, 0.81709f, 0.96875f, 1.f);
	Particle.dAge = { 0.0 };
	Particle.dLifeTime = { 1.0 };
	Particle.isAlive = true;
	XMStoreFloat4(&Particle.vVelocity, pGameInstance->Get_RandomVectorInSphere(10.f));
	Particle.vVelocity.y = 20.f;
	Particle.vAccel = _float4(0.f, -30.f, 0.f, 0.f);

	XMStoreFloat4x4(&Particle.WorldMatrix, XMMatrixScaling(5.f, 5.f, 5.f));

	Safe_Release(pGameInstance);
}

void CExplodeParticle::Render_Effect(_fvector vEffectPos)
{
	Reset_Effect();

	m_isRender = true;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vEffectPos);
}

HRESULT CExplodeParticle::Add_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	m_iParticleNum = { 30 };
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Spark_Particle"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CExplodeParticle::SetUp_ShaderResources()
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

_bool CExplodeParticle::IsParticleFinished()
{
	_bool isFinished = { true };

	for (auto& Particle : m_Particles)
	{
		// 하나라도 살아있으면 
		if (true == Particle.isAlive)
			isFinished = false;
	}

	return isFinished;
}

CExplodeParticle* CExplodeParticle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CExplodeParticle* pInstance = new CExplodeParticle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CExplodeParticle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CExplodeParticle::Clone(void* pArg)
{
	CExplodeParticle* pInstance = new CExplodeParticle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CExplodeParticle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CExplodeParticle::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
}
