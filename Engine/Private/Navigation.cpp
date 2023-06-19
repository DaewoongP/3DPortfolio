#include "..\Public\Navigation.h"
#include "Cell.h"

#ifdef _DEBUG
#include "Shader.h"
#include "PipeLine.h"
#endif

CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CNavigation::CNavigation(const CNavigation& rhs)
	: CComponent(rhs)
	, m_Cells(rhs.m_Cells)
#ifdef _DEBUG
	, m_pShader(rhs.m_pShader)
#endif
{
	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);

#ifdef _DEBUG
	Safe_AddRef(m_pShader);
#endif
}

HRESULT CNavigation::Initialize_Prototype(const _tchar* pNavigationFilePath)
{
	// file read

	return S_OK;
}

HRESULT CNavigation::Initialize(void* pArg)
{
	return S_OK;
}

#ifdef _DEBUG
HRESULT CNavigation::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShader->Begin(0);

	for (auto& pCell : m_Cells)
	{
		pCell->Render();
	}

	return S_OK;
}

HRESULT CNavigation::SetUp_ShaderResources()
{
	_float4x4 WorldMatrix, ViewMatrix, ProjMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);
	m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix);
	m_pShader->Bind_Matrix("g_ViewMatrix", pPipeLine->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW));
	m_pShader->Bind_Matrix("g_ProjMatrix", pPipeLine->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ));
	
	Safe_Release(pPipeLine);

	return S_OK;
}

#endif // _DEBUG

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationFilePath)
{
	CNavigation* pInstance = new CNavigation(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(pNavigationFilePath)))
	{
		MSG_BOX("Failed to Created CNavigation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CNavigation::Clone(void* pArg)
{
	CNavigation* pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CNavigation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();
}
