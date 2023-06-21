#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CShader final : public CComponent
{
private:
	CShader(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CShader(const CShader& rhs);
	virtual ~CShader() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar * pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC * pElements, _uint iNumElements);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Begin(_uint iPassIndex);

public:
	// 텍스처에서 만든 SRV를 던져 처리
	HRESULT Bind_ShaderResource(const _char * pConstantName, ID3D11ShaderResourceView * pSRV);
	// 텍스처에서 만든 SRV를 던져 처리
	HRESULT Bind_ShaderResources(const _char * pConstantName, ID3D11ShaderResourceView * *ppSRVArray, _uint iNumTexture);

	// 쉐이더파일에 바인딩 할 행렬
	HRESULT Bind_Matrix(const _char * pConstantName, const _float4x4 * pMatrix);
	// 쉐이더파일에 바인딩 할 행렬
	HRESULT Bind_Matrices(const _char * pConstantName, const _float4x4 * pMatrix, _uint iNumMatrices);
	// 쉐이더파일에 바인딩 할 일반 변수
	HRESULT Bind_RawValue(const _char * pConstantName, const void* pData, _uint iSize);
	HRESULT	Bind_Rasterizer(const _char * pConstantName, const D3D11_RASTERIZER_DESC * pRasterizer);

private:
	ID3DX11Effect*				m_pEffect = { nullptr };
	_uint						m_iNumPasses = { 0 };
	vector<ID3D11InputLayout*>	m_InputLayouts;

private:
	ID3DX11EffectRasterizerVariable*	m_pRasterizer = { nullptr };

public:
	static CShader* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC * pElements, _uint iNumElements);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END