#include "..\Public\Shader.h"

CShader::CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{

}

CShader::CShader(const CShader& rhs)
	: CComponent(rhs)
	, m_pEffect(rhs.m_pEffect)
	, m_iNumPasses(rhs.m_iNumPasses)
	, m_InputLayouts(rhs.m_InputLayouts)
{
	for (auto& InputLayout : m_InputLayouts)
		Safe_AddRef(InputLayout);
	Safe_AddRef(m_pEffect);
}

HRESULT CShader::Initialize_Prototype(const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
	_uint		iHlslFlag = { 0 };
#ifdef _DEBUG
	iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif

	if (FAILED(D3DX11CompileEffectFromFile(pShaderFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iHlslFlag, 0, m_pDevice, &m_pEffect, nullptr)))
		return E_FAIL;

	ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		return E_FAIL;

	D3DX11_TECHNIQUE_DESC		TechniqueDesc;
	ZeroMemory(&TechniqueDesc, sizeof TechniqueDesc);

	pTechnique->GetDesc(&TechniqueDesc);

	m_iNumPasses = TechniqueDesc.Passes;

	for (_uint i = 0; i < m_iNumPasses; ++i)
	{
		ID3D11InputLayout* pInputLayout = { nullptr };

		ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex(i);

		D3DX11_PASS_DESC	PassDesc;
		ZeroMemory(&PassDesc, sizeof PassDesc);
		pPass->GetDesc(&PassDesc);

		if (FAILED(m_pDevice->CreateInputLayout(pElements, iNumElements, 
			PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pInputLayout)))
			return E_FAIL;
		if (nullptr == pInputLayout)
			return E_FAIL;

		m_InputLayouts.emplace_back(pInputLayout);
	}


	return S_OK;
}

HRESULT CShader::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CShader::Begin(_uint iPassIndex)
{
	if (iPassIndex >= m_iNumPasses)
		return E_FAIL;

	ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		return E_FAIL;

	pTechnique->GetPassByIndex(iPassIndex)->Apply(0, m_pContext);

	m_pContext->IASetInputLayout(m_InputLayouts[iPassIndex]);

	return S_OK;
}

HRESULT CShader::Set_WVPMatrix(_matrix mat)
{
	m_pWVP = m_pEffect->GetVariableByName("g_WVPMatrix")->AsMatrix();
	if (FAILED(m_pWVP->SetMatrix(reinterpret_cast<_float*>(&mat))))
		return E_FAIL;

	return S_OK;
}

HRESULT CShader::Set_Rasterizer(const D3D11_RASTERIZER_DESC* pRasterizer)
{
	ID3D11RasterizerState* pState = { nullptr };
	m_pDevice->CreateRasterizerState(pRasterizer, &pState);

	if (nullptr == pState)
		return E_FAIL;

	m_pContext->RSSetState(pState);
	m_pRasterizer = m_pEffect->GetVariableByName("g_Rasterizer")->AsRasterizer();

	if (FAILED(m_pRasterizer->SetRasterizerState(0, pState)))
		return E_FAIL;

	Safe_Release(pState);

	return S_OK;
}

HRESULT CShader::Set_Texture()
{
	static const uint32_t s_pixel = 0xffc99aff;

	D3D11_SUBRESOURCE_DATA initData = { &s_pixel, sizeof(uint32_t), 0 };

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = desc.Height = desc.MipLevels = desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	ID3D11Texture2D* tex;
	// tex변수를 만들기위한 함수인데.
	// 이 변수를 만드려면 D3D11_TEXTURE2D_DESC, D3D11_SUBRESOURCE_DATA 필요
	m_pDevice->CreateTexture2D(&desc, &initData, &tex);

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;

	ID3D11ShaderResourceView* pSRV = { nullptr };

	m_pDevice->CreateShaderResourceView(tex, &SRVDesc, &pSRV);

	// 그 hlsl 변수 가져오기
	m_pTexture = m_pEffect->GetVariableByName("g_Texture")->AsShaderResource();

	// 텍스처 리소스세팅
	m_pTexture->SetResource(pSRV);

	Safe_Release(tex);
	Safe_Release(pSRV);

	return S_OK;
}



CShader* CShader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
	CShader* pInstance = new CShader(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pShaderFilePath, pElements, iNumElements)))
	{
		MSG_BOX("Failed to Created CShader");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CShader::Clone(void* pArg)
{
	CShader* pInstance = new CShader(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CShader");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CShader::Free()
{
	__super::Free();

	for (auto& InputLayout : m_InputLayouts)
		Safe_Release(InputLayout);
	m_InputLayouts.clear();

	Safe_Release(m_pEffect);
}
