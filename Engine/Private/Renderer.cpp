#include "..\Public\Renderer.h"
#include "GameObject.h"
#include "RenderTarget_Manager.h"
#include "Light_Manager.h"
#include "PipeLine.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"

#ifdef _DEBUG
#include "Input_Device.h"
#endif // _DEBUG


CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
	, m_pRenderTarget_Manager( CRenderTarget_Manager::GetInstance() )
	, m_pLight_Manager( CLight_Manager::GetInstance() )
{
	Safe_AddRef(m_pRenderTarget_Manager);
	Safe_AddRef(m_pLight_Manager);
}


HRESULT CRenderer::Initialize_Prototype()
{
	if (nullptr == m_pRenderTarget_Manager)
		return E_FAIL;

	_uint				iNumViews = { 1 };
	D3D11_VIEWPORT		ViewportDesc;

	m_pContext->RSGetViewports(&iNumViews, &ViewportDesc);

	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Diffuse"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Normal"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Depth"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Emissive"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Shade"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Specular"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	_uint		iShadowMapCX = (_uint)ViewportDesc.Width;
	_uint		iShadowMapCY = (_uint)ViewportDesc.Height;

	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_ShadowMap"), iShadowMapCX, iShadowMapCY, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* ID3D11Resource */
	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZEROMEM(&TextureDesc);
	
	TextureDesc.Width = iShadowMapCX;
	TextureDesc.Height = iShadowMapCY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pShadowDepthTexture2D)))
		return E_FAIL;
	
	/* DepthStencil */
	D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc;
	ZEROMEM(&DepthStencilViewDesc);

	DepthStencilViewDesc.Format = TextureDesc.Format;
	DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DepthStencilViewDesc.Texture2D.MipSlice = 0;

	if (FAILED(m_pDevice->CreateDepthStencilView(m_pShadowDepthTexture2D, &DepthStencilViewDesc, &m_pDSV)))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Emissive"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Lights"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Lights"), TEXT("Target_Specular"))))
		return E_FAIL; 
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_LightDepth"), TEXT("Target_ShadowMap"))))
		return E_FAIL;

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	m_WorldMatrix._11 = ViewportDesc.Width;
	m_WorldMatrix._22 = ViewportDesc.Height;
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

#ifdef _DEBUG 
	if (FAILED(m_pRenderTarget_Manager->Ready_Debug(TEXT("Target_Diffuse"), 80.f, 80.f, 160.f, 160.f)))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Ready_Debug(TEXT("Target_Normal"), 80.f, 240.f, 160.f, 160.f)))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Ready_Debug(TEXT("Target_Depth"), 80.f, 400.f, 160.f, 160.f)))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Ready_Debug(TEXT("Target_Emissive"), 80.f, 560.f, 160.f, 160.f)))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Ready_Debug(TEXT("Target_Shade"), 240.f, 80.f, 160.f, 160.f)))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Ready_Debug(TEXT("Target_Specular"), 240.f, 240.f, 160.f, 160.f)))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Ready_Debug(TEXT("Target_ShadowMap"), 240.f, 400.f, 160.f, 160.f)))
		return E_FAIL;
#endif // _DEBUG

	return S_OK;
}

void CRenderer::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject* pGameObject)
{
	if (eRenderGroup < RENDERGROUP::RENDER_PRIORITY ||
		eRenderGroup >= RENDERGROUP::RENDER_END ||
		nullptr == pGameObject)
	{
		MSG_BOX("Failed Add RenderGroup");
		return;
	}

	m_RenderObjects[eRenderGroup].push_back(pGameObject);

	Safe_AddRef(pGameObject);
}

#ifdef _DEBUG
HRESULT CRenderer::Add_DebugGroup(CComponent* pDebugCom)
{
	m_DebugObject.push_back(pDebugCom);

	Safe_AddRef(pDebugCom);

	return S_OK;
}
#endif // _DEBUG

HRESULT CRenderer::Draw_RenderGroup()
{
	if (FAILED(Render_Priority()))
		return E_FAIL;
	if (FAILED(Render_LightDepth()))
		return E_FAIL;
	if (FAILED(Render_NonBlend()))
		return E_FAIL;
	if (FAILED(Render_Lights()))
		return E_FAIL;
	if (FAILED(Render_Deferred()))
		return E_FAIL;
	if (FAILED(Render_NonLight()))
		return E_FAIL;
	if (FAILED(Render_Blend()))
		return E_FAIL;
	if (FAILED(Render_Screen()))
		return E_FAIL;
	if (FAILED(Render_UI()))
		return E_FAIL;

#ifdef _DEBUG
	CInput_Device* pInput_Device = CInput_Device::GetInstance();
	Safe_AddRef(pInput_Device);
	if (pInput_Device->Get_DIKeyState(DIK_F1, CInput_Device::KEY_DOWN))
	{
		if (true == m_isDebugRender)
			m_isDebugRender = false;
		else
			m_isDebugRender = true;
	}

	if (true == m_isDebugRender)
	{
		if (FAILED(Render_Debug()))
			return E_FAIL;
	}

	Safe_Release(pInput_Device);
#endif // _DEBUG
	return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_PRIORITY])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_PRIORITY].clear();

	return S_OK;
}

HRESULT CRenderer::Render_LightDepth()
{
	if (nullptr == m_pRenderTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_LightDepth"))))
		return E_FAIL;
	// 그릴 렌더타겟(Begin 에서 생성함), 원래 깊이 저장
	m_pContext->OMGetRenderTargets(1, &m_pRTV, &m_pOriginal_DSV);
	// 받아온 렌더타겟, 깊이 저장할 DSV
	m_pContext->OMSetRenderTargets(1, &m_pRTV, m_pDSV);
	// depth 1로 초기화
	m_pContext->ClearDepthStencilView(m_pDSV, D3D11_CLEAR_DEPTH, 1.f, 0);

	// 깊이 렌더링
	for (auto& pGameObject : m_RenderObjects[RENDER_LIGHTDEPTH])
	{
		if (nullptr != pGameObject)
			pGameObject->Render_LightDepth();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_LIGHTDEPTH].clear();
	
	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	Safe_Release(m_pRTV);
	Safe_Release(m_pOriginal_DSV);

	return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
	if (nullptr == m_pRenderTarget_Manager)
		return E_FAIL;

	/* Diffuse(0) + Normal(1) */
	if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_GameObjects"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RENDER_NONBLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_NONBLEND].clear();

	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Lights()
{
	if (nullptr == m_pRenderTarget_Manager)
		return E_FAIL;

	/* Shade(0) + Specular(1) */
	if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Lights"))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", pPipeLine->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", pPipeLine->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", pPipeLine->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_fCamFar", pPipeLine->Get_CamFar(), sizeof(_float))))
		return E_FAIL;

	Safe_Release(pPipeLine);

	/* 노멀 렌더타겟을 셰이더 전역으로 던진다.*/
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Normal"), m_pShader, "g_NormalTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
		return E_FAIL;

	/* 빛 정보를 던진다. */
	m_pLight_Manager->Render_Lights(m_pShader, m_pVIBuffer);

	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Deferred()
{
	// 백버퍼에 최종적으로 그려낸다.

	/* Diffuse * Shader + Specular */
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Diffuse"), m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Shade"), m_pShader, "g_ShadeTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Specular"), m_pShader, "g_SpecularTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Emissive"), m_pShader, "g_EmissiveTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_ShadowMap"), m_pShader, "g_LightDepthTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	_float4x4 LightViewMatrix;
	XMStoreFloat4x4(&LightViewMatrix, XMMatrixLookAtLH(XMVectorSet(37.f, 5.f, 90.f, 1.f), XMVectorSet(40.f, 0.f, 90.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	if (FAILED(m_pShader->Bind_Matrix("g_LightViewMatrix", &LightViewMatrix)))
		return E_FAIL;

	// GrayScale
	if (FAILED(m_pShader->Bind_RawValue("g_isGrayScale", &m_isGrayScale, sizeof(_bool))))
		return E_FAIL;
	// RedScale
	if (FAILED(m_pShader->Bind_RawValue("g_isRedScale", &m_isRedScale, sizeof(_bool))))
		return E_FAIL;

	m_pShader->Begin(4);

	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_NONLIGHT])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_NONLIGHT].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
	Sort_Blend();

	for (auto& pGameObject : m_RenderObjects[RENDER_BLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_BLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Screen()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_SCREEN])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_SCREEN].clear();

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_UI])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_UI].clear();

	return S_OK;
}

HRESULT CRenderer::Sort_Blend()
{
	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	_vector vCamPos = XMLoadFloat4(pPipeLine->Get_CamPosition());

	m_RenderObjects[RENDER_BLEND].sort([vCamPos](const CGameObject* pSour, const CGameObject* pDest) {
		_vector vSourPos = pSour->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		_vector vDestPos = pDest->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		// 내림차순 (멀리있는거부터 그림.)
		if (XMVectorGetX(XMVector3Length(vSourPos - vCamPos)) > XMVectorGetX(XMVector3Length(vDestPos - vCamPos)))
			return true;
		return false;
		});

	Safe_Release(pPipeLine);

	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderer::Render_Debug()
{
	if (nullptr == m_pRenderTarget_Manager)
		return E_FAIL;

	for (auto& pDebugCom : m_DebugObject)
	{
		if (nullptr != pDebugCom &&
			true == m_isDebugRender)
			pDebugCom->Render();

		Safe_Release(pDebugCom);
	}

	m_DebugObject.clear();

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Render_Debug(TEXT("MRT_GameObjects"), m_pShader, m_pVIBuffer)))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Render_Debug(TEXT("MRT_Lights"), m_pShader, m_pVIBuffer)))
		return E_FAIL;

	return S_OK;
}

#endif // _DEBUG

CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderer* pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CRenderer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CRenderer::Clone(void* pArg)
{
	AddRef();

	return this;
}

void CRenderer::Free()
{
	__super::Free();

	for (auto& RenderList : m_RenderObjects)
	{
		for (auto& pGameObject : RenderList)
			Safe_Release(pGameObject);

		RenderList.clear();
	}
#ifdef _DEBUG
	for (auto& pDebugCom : m_DebugObject)
		Safe_Release(pDebugCom);

	m_DebugObject.clear();
#endif // _DEBUG


	Safe_Release(m_pRenderTarget_Manager);
	Safe_Release(m_pLight_Manager);

	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);
	
	Safe_Release(m_pDSV);
	Safe_Release(m_pShadowDepthTexture2D);
}
