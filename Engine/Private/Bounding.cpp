#include "..\Public\Bounding.h"
#include "PipeLine.h"

CBounding::CBounding(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CBounding::CBounding(const CBounding& rhs)
	: m_pDevice(rhs.m_pDevice)
	, m_pContext(rhs.m_pContext)
	, m_isCloned(true)
#ifdef _DEBUG
	, m_pPrimitiveBatch(rhs.m_pPrimitiveBatch)
	, m_pEffect(rhs.m_pEffect)
	, m_pInputLayout(rhs.m_pInputLayout)
#endif // _DEBUG
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

#ifdef _DEBUG
	Safe_AddRef(m_pInputLayout);
#endif // _DEBUG
}

HRESULT CBounding::Initialize_Prototype()
{
#ifdef _DEBUG
	m_pPrimitiveBatch = new PrimitiveBatch<DirectX::VertexPositionColor>(m_pContext);
	m_pEffect = new DirectX::BasicEffect(m_pDevice);

	NULL_CHECK_RETURN_MSG(m_pPrimitiveBatch, E_FAIL, TEXT("Failed Create PrimitiveBatch"));
	NULL_CHECK_RETURN_MSG(m_pEffect, E_FAIL, TEXT("Failed Create DirectX::BasicEffect"));

	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCode = { nullptr };
	size_t		iShaderByteCodeLength = { 0 };

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

	FAILED_CHECK_RETURN_MSG(m_pDevice->CreateInputLayout(DirectX::VertexPositionColor::InputElements, DirectX::VertexPositionColor::InputElementCount,
		pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout), E_FAIL, TEXT("Failed Create InputLayout"));
#endif // _DEBUG

	return S_OK;
}

HRESULT CBounding::Begin()
{
	if (nullptr == m_pPrimitiveBatch)
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

	return S_OK;
}

HRESULT CBounding::End()
{
	m_pPrimitiveBatch->End();

	return S_OK;
}

void CBounding::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

#ifdef _DEBUG
	if (false == m_isCloned)
	{
		// 직접 동적할당 처리하고 얕은복사 처리한거기 때문에 원본에서만 삭제해줌.
		Safe_Delete(m_pPrimitiveBatch);
		Safe_Delete(m_pEffect);
	}
	Safe_Release(m_pInputLayout);
#endif // _DEBUG
}
