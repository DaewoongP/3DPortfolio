#include "..\Public\Collider.h"
#include "PipeLine.h"

CCollider::CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CCollider::CCollider(const CCollider& rhs)
	: CComponent(rhs)
#ifdef _DEBUG
	, m_pPrimitiveBatch(rhs.m_pPrimitiveBatch)
	, m_pEffect(rhs.m_pEffect)
	, m_pInputLayout(rhs.m_pInputLayout)
#endif // _DEBUG
{

#ifdef _DEBUG
	Safe_AddRef(m_pInputLayout);
#endif // _DEBUG

}

HRESULT CCollider::Initialize_Prototype(TYPE eColliderType)
{
	switch (eColliderType)
	{
	case TYPE_SPHERE:
		m_pBounding = CBounding_Sphere::Create(m_pDevice, m_pContext);
		break;
	case TYPE_AABB:
		//m_pBounding = CBounding_AABB::Create(m_pDevice, m_pContext);
		break;
	case TYPE_OBB:
		break;
	}

#ifdef _DEBUG
	m_pPrimitiveBatch = new PrimitiveBatch<DirectX::VertexPositionColor>(m_pContext);
	m_pEffect = new DirectX::BasicEffect(m_pDevice);

	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCode = { nullptr };
	size_t		iShaderByteCodeLength = { 0 };

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

	if (FAILED(m_pDevice->CreateInputLayout(DirectX::VertexPositionColor::InputElements, DirectX::VertexPositionColor::InputElementCount,
		pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout)))
		return E_FAIL;
#endif // _DEBUG

	return S_OK;
}

HRESULT CCollider::Initialize(CBounding* pBounding, void* pArg)
{
	m_pBounding = pBounding->Clone(pArg);
	
	if (nullptr == m_pBounding)
		return E_FAIL;

	return S_OK;
}

void CCollider::Tick(_fmatrix TransformMatrix)
{
	if (nullptr == m_pBounding)
		return;

	m_pBounding->Tick(TransformMatrix);
}

#ifdef _DEBUG
HRESULT CCollider::Render()
{
	if (nullptr == m_pBounding ||
		nullptr == m_pPrimitiveBatch)
		return E_FAIL;

	// 바운딩 자체에서 DirectX Bounding 객체 자체를 변환시켜주기 때문에 여기서 처리 할 필요가 없음.
	// 오히려 여기서 처리하면 렌더링할때 처리했기 때문에 나중에 충돌 처리할때도 따로 처리함수를 만들어 줘야함.
	m_pEffect->SetWorld(XMMatrixIdentity());
	
	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	m_pEffect->SetView(pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));

	Safe_Release(pPipeLine);
	// 쉐이더 Apply
	m_pEffect->Apply(m_pContext);
	// InputLayout 세팅
	m_pContext->IASetInputLayout(m_pInputLayout);
	// 정점 세팅
	m_pPrimitiveBatch->Begin();

	m_pBounding->Render(m_pPrimitiveBatch);

	m_pPrimitiveBatch->End();

	return S_OK;
}
#endif // _DEBUG

CCollider* CCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eColliderType)
{
	CCollider* pInstance = new CCollider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eColliderType)))
	{
		MSG_BOX("Failed to Created CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CCollider::Clone(void* pArg)
{
	CCollider* pInstance = new CCollider(*this);

	if (FAILED(pInstance->Initialize(m_pBounding, pArg)))
	{
		MSG_BOX("Failed to Cloned CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider::Free()
{
	__super::Free();

#ifdef _DEBUG
	if (false == m_isCloned)
	{
		// 직접 동적할당 처리하고 얕은복사 처리한거기 때문에 원본에서만 삭제해줌.
		Safe_Delete(m_pPrimitiveBatch);
		Safe_Delete(m_pEffect);
	}
	Safe_Release(m_pInputLayout);
#endif // _DEBUG

	Safe_Release(m_pBounding);
}
