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

	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_ShadowMap"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(Create_DepthTexture((_uint)ViewportDesc.Width * 10, (_uint)ViewportDesc.Height * 10)))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_PostProcessing"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Effect"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Bloom"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Blur"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
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
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_PostProcessing"), TEXT("Target_PostProcessing"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Effects"), TEXT("Target_Effect"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Bloom"), TEXT("Target_Bloom"))))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Blur"), TEXT("Target_Blur"))))
		return E_FAIL;

	if (FAILED(Add_Components()))
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
	if (FAILED(m_pRenderTarget_Manager->Ready_Debug(TEXT("Target_PostProcessing"), 240.f, 560.f, 160.f, 160.f)))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Ready_Debug(TEXT("Target_Bloom"), 400.f, 80.f, 160.f, 160.f)))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Ready_Debug(TEXT("Target_Blur"), 400.f, 240.f, 160.f, 160.f)))
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
	if (FAILED(m_pRenderTarget_Manager->Begin_PreMRT(m_pContext, TEXT("MRT_PostProcessing"))))
		return E_FAIL;

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

	if (FAILED(m_pRenderTarget_Manager->End_PreMRT(m_pContext)))
		return E_FAIL;
	
	if (FAILED(Render_Bloom()))
		return E_FAIL;
	if (FAILED(Render_Blur()))
		return E_FAIL;
	if (FAILED(Render_PostProcessing()))
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

	if (FAILED(m_pDeferredShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pDeferredShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pDeferredShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	if (FAILED(m_pDeferredShader->Bind_Matrix("g_ViewMatrixInv", pPipeLine->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pDeferredShader->Bind_Matrix("g_ProjMatrixInv", pPipeLine->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pDeferredShader->Bind_RawValue("g_vCamPosition", pPipeLine->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pDeferredShader->Bind_RawValue("g_fCamFar", pPipeLine->Get_CamFar(), sizeof(_float))))
		return E_FAIL;

	Safe_Release(pPipeLine);

	/* 노멀 렌더타겟을 셰이더 전역으로 던진다.*/
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Normal"), m_pDeferredShader, "g_NormalTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Depth"), m_pDeferredShader, "g_DepthTexture")))
		return E_FAIL;

	/* 빛 정보를 던진다. */
	m_pLight_Manager->Render_Lights(m_pDeferredShader, m_pDeferredBuffer);

	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Deferred()
{
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Diffuse"), m_pDeferredShader, "g_DiffuseTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Shade"), m_pDeferredShader, "g_ShadeTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Specular"), m_pDeferredShader, "g_SpecularTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Depth"), m_pDeferredShader, "g_DepthTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Emissive"), m_pDeferredShader, "g_EmissiveTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_ShadowMap"), m_pDeferredShader, "g_LightDepthTexture")))
		return E_FAIL;

	if (FAILED(m_pDeferredShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pDeferredShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pDeferredShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	if (FAILED(m_pDeferredShader->Bind_RawValue("g_fCamFar", pPipeLine->Get_CamFar(), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pDeferredShader->Bind_RawValue("g_fLightFar", pPipeLine->Get_LightFar(), sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pDeferredShader->Bind_Matrix("g_ViewMatrixInv", pPipeLine->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pDeferredShader->Bind_Matrix("g_ProjMatrixInv", pPipeLine->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pDeferredShader->Bind_Matrix("g_LightViewMatrix", pPipeLine->Get_LightDepthFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pDeferredShader->Bind_Matrix("g_LightProjMatrix", pPipeLine->Get_LightDepthFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	Safe_Release(pPipeLine);

	m_pDeferredShader->Begin(4);

	m_pDeferredBuffer->Render();

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
	if (FAILED(Sort_Blend()))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RENDER_BLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_BLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Bloom()
{
	if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Bloom"))))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_PostProcessing"), m_pPostProcessingShader, "g_PostProcessingTexture")))
		return E_FAIL;

	if (FAILED(m_pPostProcessingShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pPostProcessingShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pPostProcessingShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	m_pPostProcessingShader->Begin(2);

	m_pPostProcessingBuffer->Render();

	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Blur()
{
	if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Blur"))))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_PostProcessing"), m_pPostProcessingShader, "g_PostProcessingTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Bloom"), m_pPostProcessingShader, "g_BloomTexture")))
		return E_FAIL;

	if (FAILED(m_pPostProcessingShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pPostProcessingShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pPostProcessingShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	m_pPostProcessingShader->Begin(1);

	m_pPostProcessingBuffer->Render();

	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_PostProcessing()
{
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_PostProcessing"), m_pPostProcessingShader, "g_PostProcessingTexture")))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResourceView(TEXT("Target_Blur"), m_pPostProcessingShader, "g_BlurTexture")))
		return E_FAIL;

	if (FAILED(m_pPostProcessingShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pPostProcessingShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pPostProcessingShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);


	Safe_Release(pPipeLine);

#ifdef _DEBUG
	CInput_Device* pInput_Device = CInput_Device::GetInstance();
	Safe_AddRef(pInput_Device);
	if (pInput_Device->Get_DIKeyState(DIK_F2, CInput_Device::KEY_DOWN))
	{
		if (true == m_isBlur)
			m_isBlur = false;
		else
			m_isBlur = true;
	}
	Safe_Release(pInput_Device);
	// Blur
	if (FAILED(m_pPostProcessingShader->Bind_RawValue("g_isBlur", &m_isBlur, sizeof(_bool))))
		return E_FAIL;
#endif // _DEBUG

	// GrayScale
	if (FAILED(m_pPostProcessingShader->Bind_RawValue("g_isGrayScale", &m_isGrayScale, sizeof(_bool))))
		return E_FAIL;
	// RedScale
	if (FAILED(m_pPostProcessingShader->Bind_RawValue("g_isRedScale", &m_isRedScale, sizeof(_bool))))
		return E_FAIL;

	m_pPostProcessingShader->Begin(0);

	m_pPostProcessingBuffer->Render();

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
	if (FAILED(Sort_UI()))
		return E_FAIL;

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

	Safe_Release(pPipeLine);

	m_RenderObjects[RENDER_BLEND].sort([vCamPos](const CGameObject* pSour, const CGameObject* pDest) {
		_vector vSourPos = pSour->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		_vector vDestPos = pDest->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		// 내림차순 (멀리있는거부터 그림.)
		if (XMVectorGetX(XMVector3Length(vSourPos - vCamPos)) > XMVectorGetX(XMVector3Length(vDestPos - vCamPos)))
			return true;
		return false;
		});

	return S_OK;
}

HRESULT CRenderer::Sort_UI()
{
	m_RenderObjects[RENDER_UI].sort([](const CGameObject* pSour, const CGameObject* pDest) {
		_float fSourZ = XMVectorGetZ(pSour->Get_Transform()->Get_State(CTransform::STATE_POSITION));
		_float fDestZ = XMVectorGetZ(pDest->Get_Transform()->Get_State(CTransform::STATE_POSITION));
		// 내림차순 (멀리있는거부터 그림.)
		if (fSourZ > fDestZ)
			return true;
		return false;
		});

	return S_OK;
}

HRESULT CRenderer::Add_Components()
{
	m_pDeferredShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements);
	if (nullptr == m_pDeferredShader)
		return E_FAIL;

	m_pDeferredBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pDeferredBuffer)
		return E_FAIL;

	m_pPostProcessingShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_PostProcessing.hlsl"), VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements);
	if (nullptr == m_pPostProcessingShader)
		return E_FAIL;

	m_pPostProcessingBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pPostProcessingBuffer)
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Create_DepthTexture(_uint iSizeX, _uint iSizeY)
{
	/* ID3D11Resource */
	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZEROMEM(&TextureDesc);

	// 최대 텍스처 사이즈 16384
	TextureDesc.Width = iSizeX;
	TextureDesc.Height = iSizeY;
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
	DepthStencilViewDesc.Flags = 0;
	DepthStencilViewDesc.Format = TextureDesc.Format;
	DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DepthStencilViewDesc.Texture2D.MipSlice = 0;
	if (FAILED(m_pDevice->CreateDepthStencilView(m_pShadowDepthTexture2D, &DepthStencilViewDesc, &m_pDSV)))
		return E_FAIL;

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

	if (FAILED(m_pDeferredShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pDeferredShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Render_Debug(TEXT("MRT_GameObjects"), m_pDeferredShader, m_pDeferredBuffer)))
		return E_FAIL;
	if (FAILED(m_pRenderTarget_Manager->Render_Debug(TEXT("MRT_Lights"), m_pDeferredShader, m_pDeferredBuffer)))
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

	Safe_Release(m_pDeferredShader);
	Safe_Release(m_pDeferredBuffer);
	Safe_Release(m_pPostProcessingShader);
	Safe_Release(m_pPostProcessingBuffer);
	
	Safe_Release(m_pDSV);
	Safe_Release(m_pShadowDepthTexture2D);
}
