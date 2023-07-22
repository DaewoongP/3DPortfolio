#include "..\Public\GameInstance.h"
#include "Frustum.h"
#include "Calculator.h"
#include "Font_Manager.h"
#include "Level_Manager.h"
#include "Timer_Manager.h"
#include "Graphic_Device.h"
#include "Object_Manager.h"
#include "RenderTarget_Manager.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
	: m_pGraphic_Device{ CGraphic_Device::GetInstance() }
	, m_pLevel_Manager{ CLevel_Manager::GetInstance() }
	, m_pObject_Manager{ CObject_Manager::GetInstance() }
	, m_pTimer_Manager{ CTimer_Manager::GetInstance() }
	, m_pComponent_Manager{ CComponent_Manager::GetInstance() }
	, m_pInput_Device{ CInput_Device::GetInstance() }
	, m_pPipeLine{ CPipeLine::GetInstance() }
	, m_pCalculator{ CCalculator::GetInstance() }
	, m_pCollision_Manager{ CCollision_Manager::GetInstance() }
	, m_pFont_Manager{ CFont_Manager::GetInstance() }
	, m_pFrustum{ CFrustum::GetInstance() }
	, m_pRenderTarget_Manager{ CRenderTarget_Manager::GetInstance() }
	, m_pLight_Manager{ CLight_Manager::GetInstance() }
{
	Safe_AddRef(m_pFrustum);
	Safe_AddRef(m_pFont_Manager);
	Safe_AddRef(m_pCalculator);
	Safe_AddRef(m_pPipeLine);
	Safe_AddRef(m_pGraphic_Device);
	Safe_AddRef(m_pLevel_Manager);
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pTimer_Manager);
	Safe_AddRef(m_pComponent_Manager);
	Safe_AddRef(m_pInput_Device);
	Safe_AddRef(m_pCollision_Manager);
	Safe_AddRef(m_pRenderTarget_Manager);
	Safe_AddRef(m_pLight_Manager);
}

HRESULT CGameInstance::Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const GRAPHICDESC& GraphicDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext)
{
	NULL_CHECK_RETURN_MSG(m_pGraphic_Device, E_FAIL, TEXT("Graphic Device NULL"));

	if (FAILED(m_pGraphic_Device->Ready_Graphic_Device(GraphicDesc.hWnd, GraphicDesc.eWinMode, GraphicDesc.iViewportSizeX, GraphicDesc.iViewportSizeY, ppDevice, ppContext)))
		return E_FAIL;

	if (FAILED(Reserve_Engine(iNumLevels)))
		return E_FAIL;

	if (FAILED(m_pInput_Device->Ready_Input_Device(hInst, GraphicDesc.hWnd)))
		return E_FAIL;

	if (FAILED(m_pFrustum->Initialize()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGameInstance::Reserve_Engine(_uint iNumLevels)
{
	if (FAILED(m_pObject_Manager->Reserve_Containers(iNumLevels)))
		return E_FAIL;

	if (FAILED(m_pComponent_Manager->Reserve_Containers(iNumLevels)))
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Tick_Engine(_double dTimeDelta)
{
	NULL_CHECK_RETURN_MSG(m_pLevel_Manager, , TEXT("Level_Manager NULL"));
	NULL_CHECK_RETURN_MSG(m_pObject_Manager, , TEXT("Object_Manager NULL"));
	NULL_CHECK_RETURN_MSG(m_pPipeLine, , TEXT("PipeLine NULL"));
	NULL_CHECK_RETURN_MSG(m_pInput_Device, , TEXT("Input_Device NULL"));

	m_pInput_Device->Tick();
	m_pObject_Manager->Tick(dTimeDelta);
	m_pPipeLine->Tick();
	m_pFrustum->Tick();
	m_pObject_Manager->Late_Tick(dTimeDelta);
	m_pCollision_Manager->Tick();
	m_pLevel_Manager->Tick(dTimeDelta);
}

void CGameInstance::Clear_LevelResources(_uint iLevelIndex)
{
	NULL_CHECK_RETURN_MSG(m_pObject_Manager, , TEXT("Object_Manager NULL"));
	NULL_CHECK_RETURN_MSG(m_pComponent_Manager, , TEXT("Component_Manager NULL"));

	m_pObject_Manager->Clear_LevelResources(iLevelIndex);
	m_pComponent_Manager->Clear_LevelResources(iLevelIndex);
}

HRESULT CGameInstance::Clear_BackBuffer_View(_float4 vClearColor)
{
	NULL_CHECK_RETURN_MSG(m_pGraphic_Device, E_FAIL, TEXT("Graphic_Device NULL"));

	return m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	NULL_CHECK_RETURN_MSG(m_pGraphic_Device, E_FAIL, TEXT("Graphic_Device NULL"));
	
	return m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	NULL_CHECK_RETURN_MSG(m_pGraphic_Device, E_FAIL, TEXT("Graphic_Device NULL"));

	return m_pGraphic_Device->Present();
}

HRESULT CGameInstance::ResetRenderTargets()
{
	NULL_CHECK_RETURN_MSG(m_pGraphic_Device, E_FAIL, TEXT("Graphic_Device NULL"));

	return m_pGraphic_Device->ResetRenderTargets();
}

HRESULT CGameInstance::Resize_Buffer(_uint& ResizeWidth, _uint&  ResizeHeight)
{
	NULL_CHECK_RETURN_MSG(m_pGraphic_Device, E_FAIL, TEXT("Graphic_Device NULL"));

	return m_pGraphic_Device->Resize_Buffer(ResizeWidth, ResizeHeight);
}

HRESULT CGameInstance::Add_Timer(const _tchar* pTimerTag)
{
	NULL_CHECK_RETURN_MSG(m_pTimer_Manager, E_FAIL, TEXT("Timer_Manager NULL"));

	return m_pTimer_Manager->Add_Timer(pTimerTag);
}

void CGameInstance::Tick_Timer(const _tchar* pTimerTag)
{
	NULL_CHECK_RETURN_MSG(m_pTimer_Manager, , TEXT("Timer_Manager NULL"));

	m_pTimer_Manager->Tick_Timer(pTimerTag);
}

_double CGameInstance::Get_TimeDelta(const _tchar* pTimerTag)
{
	NULL_CHECK_RETURN_MSG(m_pTimer_Manager, 0.0, TEXT("Timer_Manager NULL"));

	return m_pTimer_Manager->Get_TimeDelta(pTimerTag);
}

void CGameInstance::Set_SlowedTime(const _tchar* pTimerTag, _double dTime)
{
	NULL_CHECK_RETURN_MSG(m_pTimer_Manager, , TEXT("Timer_Manager NULL"));

	return m_pTimer_Manager->Set_SlowedTime(pTimerTag, dTime);
}

HRESULT CGameInstance::Open_Level(_uint iLevelIndex, CLevel* pNewLevel)
{
	NULL_CHECK_RETURN_MSG(m_pLevel_Manager, E_FAIL, TEXT("Level_Manager NULL"));

	return m_pLevel_Manager->Open_Level(iLevelIndex, pNewLevel);
}

_uint CGameInstance::Get_CurrentLevelIndex() const
{
	NULL_CHECK_RETURN_MSG(m_pLevel_Manager, 0, TEXT("Level_Manager NULL"));

	return m_pLevel_Manager->Get_CurrentLevelIndex();
}

HRESULT CGameInstance::Add_Prototype(const _tchar* pPrototypeTag, CGameObject* pPrototype)
{
	NULL_CHECK_RETURN_MSG(m_pObject_Manager, E_FAIL, TEXT("Object_Manager NULL"));
	
	return m_pObject_Manager->Add_Prototype(pPrototypeTag, pPrototype);
}

HRESULT CGameInstance::Add_GameObject(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pLayerTag, const _tchar* pGameObjectTag, void* pArg)
{
	NULL_CHECK_RETURN_MSG(m_pObject_Manager, E_FAIL, TEXT("Object_Manager NULL"));

	return m_pObject_Manager->Add_GameObject(iLevelIndex, pPrototypeTag, pLayerTag, pGameObjectTag, pArg);
}

CGameObject* CGameInstance::Clone_GameObject(const _tchar* pPrototypeTag, void* pArg)
{
	NULL_CHECK_RETURN_MSG(m_pObject_Manager, nullptr, TEXT("Object_Manager NULL"));

	return m_pObject_Manager->Clone_GameObject(pPrototypeTag, pArg);
}

CGameObject* CGameInstance::Find_GameObject(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pGameObjectTag)
{
	NULL_CHECK_RETURN_MSG(m_pObject_Manager, nullptr, TEXT("Object_Manager NULL"));

	return m_pObject_Manager->Find_GameObject(iLevelIndex, pLayerTag, pGameObjectTag);
}

CLayer* CGameInstance::Find_Layer(_uint iLevelIndex, const _tchar* pLayerTag)
{
	NULL_CHECK_RETURN_MSG(m_pObject_Manager, nullptr, TEXT("Object_Manager NULL"));

	return m_pObject_Manager->Find_Layer(iLevelIndex, pLayerTag);
}

HRESULT CGameInstance::Delete_GameObject(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pGameObjectTag)
{
	NULL_CHECK_RETURN_MSG(m_pObject_Manager, E_FAIL, TEXT("Object_Manager NULL"));

	return m_pObject_Manager->Delete_GameObject(iLevelIndex, pLayerTag, pGameObjectTag);
}

HRESULT CGameInstance::Clear_Layer(_uint iLevelIndex, const _tchar* pLayerTag)
{
	NULL_CHECK_RETURN_MSG(m_pObject_Manager, E_FAIL, TEXT("Object_Manager NULL"));

	return m_pObject_Manager->Clear_Layer(iLevelIndex, pLayerTag);
}

CGameObject* CGameInstance::Get_LastGameObject()
{
	NULL_CHECK_RETURN_MSG(m_pObject_Manager, nullptr, TEXT("Object_Manager NULL"));

	return m_pObject_Manager->Get_LastGameObject();
}

HRESULT CGameInstance::Set_LastGameObject(CGameObject* pGameObject)
{
	NULL_CHECK_RETURN_MSG(m_pObject_Manager, E_FAIL, TEXT("Object_Manager NULL"));

	return m_pObject_Manager->Set_LastGameObject(pGameObject);
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, CComponent* pPrototype)
{
	NULL_CHECK_RETURN_MSG(m_pComponent_Manager, E_FAIL, TEXT("Component_Manager NULL"));

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, pPrototypeTag, pPrototype);
}

CComponent* CGameInstance::Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg)
{
	NULL_CHECK_RETURN_MSG(m_pComponent_Manager, nullptr, TEXT("Component_Manager NULL"));

	return m_pComponent_Manager->Clone_Component(iLevelIndex, pPrototypeTag, pArg);
}

HRESULT CGameInstance::Delete_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag)
{
	NULL_CHECK_RETURN_MSG(m_pComponent_Manager, E_FAIL, TEXT("Component_Manager NULL"));

	return m_pComponent_Manager->Delete_Prototype(iLevelIndex, pPrototypeTag);
}

unordered_map<const _tchar*, class CComponent*> CGameInstance::Find_PrototypesBySubTag(_uint iLevelIndex, const _tchar* pSubTag)
{
	if (nullptr == m_pComponent_Manager)
		return unordered_map<const _tchar*, class CComponent*>();

	return m_pComponent_Manager->Find_PrototypesBySubTag(iLevelIndex, pSubTag);
}

_bool	CGameInstance::Get_DIKeyState(_ubyte ubyKeyID, CInput_Device::KEYSTATE eState)
{
	NULL_CHECK_RETURN_MSG(m_pInput_Device, 0, TEXT("Input_Device NULL"));

	return m_pInput_Device->Get_DIKeyState(ubyKeyID, eState);
}

_bool	CGameInstance::Get_DIMouseState(CInput_Device::MOUSEKEYSTATE eMouseID, CInput_Device::KEYSTATE eState)
{
	NULL_CHECK_RETURN_MSG(m_pInput_Device, 0, TEXT("Input_Device NULL"));

	return m_pInput_Device->Get_DIMouseState(eMouseID, eState);
}

_long	CGameInstance::Get_DIMouseMove(CInput_Device::MOUSEMOVESTATE eMouseMoveID)
{
	NULL_CHECK_RETURN_MSG(m_pInput_Device, 0, TEXT("Input_Device NULL"));

	return m_pInput_Device->Get_DIMouseMove(eMouseMoveID);
}

void CGameInstance::Set_Transform(CPipeLine::D3DTRANSFORMSTATE eTransformState, _fmatrix TransformStateMatrix)
{
	NULL_CHECK_RETURN_MSG(m_pPipeLine, , TEXT("PipeLine NULL"));

	m_pPipeLine->Set_Transform(eTransformState, TransformStateMatrix);
}

_matrix CGameInstance::Get_TransformMatrix(CPipeLine::D3DTRANSFORMSTATE eTransformState)
{
	NULL_CHECK_RETURN_MSG(m_pPipeLine, _matrix(), TEXT("PipeLine NULL"));

	return m_pPipeLine->Get_TransformMatrix(eTransformState);
}

_float4x4* CGameInstance::Get_TransformFloat4x4(CPipeLine::D3DTRANSFORMSTATE eTransformState)
{
	NULL_CHECK_RETURN_MSG(m_pPipeLine, nullptr, TEXT("PipeLine NULL"));

	return m_pPipeLine->Get_TransformFloat4x4(eTransformState);
}

_matrix CGameInstance::Get_TransformMatrix_Inverse(CPipeLine::D3DTRANSFORMSTATE eTransformState)
{
	NULL_CHECK_RETURN_MSG(m_pPipeLine, _matrix(), TEXT("PipeLine NULL"));

	return m_pPipeLine->Get_TransformMatrix_Inverse(eTransformState);
}

_float4x4* CGameInstance::Get_TransformFloat4x4_Inverse(CPipeLine::D3DTRANSFORMSTATE eTransformState)
{
	NULL_CHECK_RETURN_MSG(m_pPipeLine, nullptr, TEXT("PipeLine NULL"));

	return m_pPipeLine->Get_TransformFloat4x4_Inverse(eTransformState);
}

_float4* CGameInstance::Get_CamPosition()
{
	NULL_CHECK_RETURN_MSG(m_pPipeLine, nullptr, TEXT("PipeLine NULL"));

	return m_pPipeLine->Get_CamPosition();
}

_float* CGameInstance::Get_CamFar()
{
	NULL_CHECK_RETURN_MSG(m_pPipeLine, nullptr, TEXT("PipeLine NULL"));

	return m_pPipeLine->Get_CamFar();
}

HRESULT CGameInstance::Get_MouseRay(ID3D11DeviceContext* pContext, HWND hWnd, _fmatrix PickingWorldMatrix_Inverse, _Inout_ _float4* vRayPos, _Inout_ _float4* vRayDir)
{
	NULL_CHECK_RETURN_MSG(m_pCalculator, E_FAIL, TEXT("Calculator NULL"));

	return m_pCalculator->Get_MouseRay(pContext, hWnd, PickingWorldMatrix_Inverse, vRayPos, vRayDir);
}

HRESULT CGameInstance::Get_WorldMouseRay(ID3D11DeviceContext* pContext, HWND hWnd, _Inout_ _float4* vRayPos, _Inout_ _float4* vRayDir)
{
	NULL_CHECK_RETURN_MSG(m_pCalculator, E_FAIL, TEXT("Calculator NULL"));

	return m_pCalculator->Get_WorldMouseRay(pContext, hWnd, vRayPos, vRayDir);
}

_bool CGameInstance::IsMouseInClient(ID3D11DeviceContext* pContext, HWND hWnd)
{
	NULL_CHECK_RETURN_MSG(m_pCalculator, false, TEXT("Calculator NULL"));

	return m_pCalculator->IsMouseInClient(pContext, hWnd);
}

HRESULT CGameInstance::Add_Collider(COLLISIONDESC::COLTYPE eCollisionType, CCollider* pCollider)
{
	if (nullptr == m_pCollision_Manager)
		return E_FAIL;

	return m_pCollision_Manager->Add_Collider(eCollisionType, pCollider);
}

HRESULT CGameInstance::Add_Fonts(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontTag, const _tchar* pFontFilePath)
{
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return m_pFont_Manager->Add_Fonts(pDevice, pContext, pFontTag, pFontFilePath);
}

HRESULT CGameInstance::Render_Font(const _tchar* pFontTag, const _tchar* pText, const _float2& Position, _fvector vColor, _float fRotation, const _float2& vOrigin, _float fScale)
{
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return m_pFont_Manager->Render_Font(pFontTag, pText, Position, vColor, fRotation, vOrigin, fScale);
}

_bool CGameInstance::isIn_WorldFrustum(_fvector vWorldPos, _float fRange)
{
	if (nullptr == m_pFrustum)
		return false;

	return m_pFrustum->isIn_WorldFrustum(vWorldPos, fRange);
}

const CLight::LIGHTDESC* CGameInstance::Get_Light(_uint iIndex)
{
	if (nullptr == m_pLight_Manager)
		return nullptr;

	return m_pLight_Manager->Get_Light(iIndex);
}

HRESULT CGameInstance::Add_Lights(const CLight::LIGHTDESC& LightDesc)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Add_Lights(LightDesc);
}

HRESULT CGameInstance::Clear_Lights()
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Clear_Lights();
}

void CGameInstance::Release_Engine()
{
	CGameInstance::GetInstance()->DestroyInstance();

	CPipeLine::GetInstance()->DestroyInstance();

	CObject_Manager::GetInstance()->DestroyInstance();

	CComponent_Manager::GetInstance()->DestroyInstance();

	CLevel_Manager::GetInstance()->DestroyInstance();

	CTimer_Manager::GetInstance()->DestroyInstance();

	CCalculator::GetInstance()->DestroyInstance();

	CFont_Manager::GetInstance()->DestroyInstance();
	
	CFrustum::GetInstance()->DestroyInstance();

	CCollision_Manager::GetInstance()->DestroyInstance();

	CRenderTarget_Manager::GetInstance()->DestroyInstance();

	CLight_Manager::GetInstance()->DestroyInstance();

	CInput_Device::GetInstance()->DestroyInstance();

	CGraphic_Device::GetInstance()->DestroyInstance();
}

void CGameInstance::Free()
{
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pRenderTarget_Manager);
	Safe_Release(m_pFrustum);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pCollision_Manager);
	Safe_Release(m_pCalculator);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pGraphic_Device);
}
