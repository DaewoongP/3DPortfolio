#include "..\Public\BulletSpark.h"
#include "GameInstance.h"

CBulletSpark::CBulletSpark(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CBulletSpark::CBulletSpark(const CBulletSpark& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBulletSpark::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBulletSpark::Initialize(void* pArg)
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

void CBulletSpark::Tick(_double dTimeDelta)
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

		XMStoreFloat4x4(&Particle.WorldMatrix, XMMatrixScaling(0.3f, 0.3f, 0.3f) * XMMatrixTranslation(vPos.x, vPos.y, vPos.z));

		ParticleMatrices.push_back(Particle.WorldMatrix);

		m_isRender = true;
	}

	m_pVIBufferCom->Tick(ParticleMatrices.data());
}

GAMEEVENT CBulletSpark::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (nullptr != m_pRendererCom &&
		true == m_isRender)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);

	return GAME_NOEVENT;
}

HRESULT CBulletSpark::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);

	m_pVIBufferCom->Render();

	return S_OK;
}

void CBulletSpark::Reset_Effect()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	for (auto& Particle : m_Particles)
	{
		Particle.dAge = { 0.0 };
		Particle.dLifeTime = { 3.0 };
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

void CBulletSpark::Render_Effect(_fvector vEffectPos)
{
	Reset_Effect();

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vEffectPos);
}

HRESULT CBulletSpark::Add_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	m_iParticleNum = { 20 };
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Spark_Particle"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBulletSpark::SetUp_ShaderResources()
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

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	_float4 vColor = _float4(1.f, 0.3f, 0.3f, 1.f);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

CBulletSpark* CBulletSpark::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBulletSpark* pInstance = new CBulletSpark(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBulletSpark");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBulletSpark::Clone(void* pArg)
{
	CBulletSpark* pInstance = new CBulletSpark(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CBulletSpark");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBulletSpark::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}
