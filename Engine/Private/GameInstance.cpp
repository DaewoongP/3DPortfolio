#include "..\Public\GameInstance.h"
#include "Graphic_Device.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Timer_Manager.h"
#include "Calculator.h"
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
{
	Safe_AddRef(m_pCalculator);
	Safe_AddRef(m_pPipeLine);
	Safe_AddRef(m_pGraphic_Device);
	Safe_AddRef(m_pLevel_Manager);
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pTimer_Manager);
	Safe_AddRef(m_pComponent_Manager);
	Safe_AddRef(m_pInput_Device);
}

HRESULT CGameInstance::Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const GRAPHICDESC& GraphicDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext)
{
	NULL_CHECK_RETURN(m_pGraphic_Device, E_FAIL);
	FAILED_CHECK_RETURN_MSG(m_pGraphic_Device->Ready_Graphic_Device(GraphicDesc.hWnd, GraphicDesc.eWinMode, GraphicDesc.iViewportSizeX, GraphicDesc.iViewportSizeY, ppDevice, ppContext), E_FAIL,
		L"Failed Ready_Graphic_Device");

	FAILED_CHECK_RETURN_MSG(Reserve_Engine(iNumLevels), E_FAIL, TEXT("Failed Reserve_Engine"));
	FAILED_CHECK_RETURN_MSG(m_pInput_Device->Ready_Input_Device(hInst, GraphicDesc.hWnd), E_FAIL, TEXT("Failed Ready Input device"));
	return S_OK;
}

HRESULT CGameInstance::Reserve_Engine(_uint iNumLevels)
{
	FAILED_CHECK_RETURN_MSG(m_pObject_Manager->Reserve_Containers(iNumLevels), E_FAIL,
		L"Failed Reserve_Containers");
	FAILED_CHECK_RETURN_MSG(m_pComponent_Manager->Reserve_Containers(iNumLevels), E_FAIL,
		L"Failed Reserve_Containers")

	return S_OK;
}

void CGameInstance::Tick_Engine(_double dTimeDelta)
{
	if (nullptr == m_pLevel_Manager ||
		nullptr == m_pObject_Manager ||
		nullptr == m_pPipeLine ||
		nullptr == m_pInput_Device)
		return;

	m_pInput_Device->Tick();
	m_pObject_Manager->Tick(dTimeDelta);
	m_pPipeLine->Tick();
	m_pObject_Manager->Late_Tick(dTimeDelta);
	m_pLevel_Manager->Tick(dTimeDelta);
}

void CGameInstance::Clear_LevelResources(_uint iLevelIndex)
{
	if (nullptr == m_pObject_Manager ||
		nullptr == m_pComponent_Manager)
		return;

	m_pObject_Manager->Clear_LevelResources(iLevelIndex);
	m_pComponent_Manager->Clear_LevelResources(iLevelIndex);
}

HRESULT CGameInstance::Clear_BackBuffer_View(_float4 vClearColor)
{
	NULL_CHECK_RETURN(m_pGraphic_Device, E_FAIL);
	
	return m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	NULL_CHECK_RETURN(m_pGraphic_Device, E_FAIL);
	
	return m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	NULL_CHECK_RETURN(m_pGraphic_Device, E_FAIL);

	return m_pGraphic_Device->Present();
}

HRESULT CGameInstance::ResetRenderTargets()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->ResetRenderTargets();
}

HRESULT CGameInstance::Resize_Buffer(_uint& ResizeWidth, _uint&  ResizeHeight)
{
	NULL_CHECK_RETURN(m_pGraphic_Device, E_FAIL);

	return m_pGraphic_Device->Resize_Buffer(ResizeWidth, ResizeHeight);
}

HRESULT CGameInstance::Add_Timer(const _tchar* pTimerTag)
{
	NULL_CHECK_RETURN(m_pTimer_Manager, E_FAIL);

	return m_pTimer_Manager->Add_Timer(pTimerTag);
}

void CGameInstance::Tick_Timer(const _tchar* pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return;

	m_pTimer_Manager->Tick_Timer(pTimerTag);
}

_double CGameInstance::Get_TimeDelta(const _tchar* pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.0;

	return m_pTimer_Manager->Get_TimeDelta(pTimerTag);
}

HRESULT CGameInstance::Open_Level(_uint iLevelIndex, CLevel* pNewLevel)
{
	NULL_CHECK_RETURN(m_pLevel_Manager, E_FAIL);

	return m_pLevel_Manager->Open_Level(iLevelIndex, pNewLevel);
}

HRESULT CGameInstance::Add_Prototype(const _tchar* pPrototypeTag, CGameObject* pPrototype)
{
	NULL_CHECK_RETURN(m_pObject_Manager, E_FAIL);
	
	return m_pObject_Manager->Add_Prototype(pPrototypeTag, pPrototype);
}

HRESULT CGameInstance::Add_GameObject(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pLayerTag, const _tchar* pGameObjectTag, void* pArg)
{
	NULL_CHECK_RETURN(m_pObject_Manager, E_FAIL);

	return m_pObject_Manager->Add_GameObject(iLevelIndex, pPrototypeTag, pLayerTag, pGameObjectTag, pArg);
}

CGameObject* CGameInstance::Find_GameObject(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pGameObjectTag)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Find_GameObject(iLevelIndex, pLayerTag, pGameObjectTag);
}

CLayer* CGameInstance::Find_Layer(_uint iLevelIndex, const _tchar* pLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Find_Layer(iLevelIndex, pLayerTag);
}

CGameObject* CGameInstance::Get_LastGameObject()
{
	if (nullptr == m_pObject_Manager)
		return nullptr;
	return m_pObject_Manager->Get_LastGameObject();
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, CComponent* pPrototype)
{
	NULL_CHECK_RETURN(m_pComponent_Manager, E_FAIL);

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, pPrototypeTag, pPrototype);
}

CComponent* CGameInstance::Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(iLevelIndex, pPrototypeTag, pArg);
}

HRESULT CGameInstance::Delete_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Delete_Prototype(iLevelIndex, pPrototypeTag);
}

unordered_map<const _tchar*, class CComponent*> CGameInstance::Find_PrototypesBySubTag(_uint iLevelIndex, const _tchar* pSubTag)
{
	if (nullptr == m_pComponent_Manager)
		return unordered_map<const _tchar*, class CComponent*>();

	return m_pComponent_Manager->Find_PrototypesBySubTag(iLevelIndex, pSubTag);
}

_byte	CGameInstance::Get_DIKeyState(_ubyte ubyKeyID, CInput_Device::KEYSTATE eState)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIKeyState(ubyKeyID, eState);
}

_byte	CGameInstance::Get_DIMouseState(CInput_Device::MOUSEKEYSTATE eMouseID, CInput_Device::KEYSTATE eState)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMouseState(eMouseID, eState);
}

_long	CGameInstance::Get_DIMouseMove(CInput_Device::MOUSEMOVESTATE eMouseMoveID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMouseMove(eMouseMoveID);
}

void CGameInstance::Set_Transform(CPipeLine::D3DTRANSFORMSTATE eTransformState, _fmatrix TransformStateMatrix)
{
	if (nullptr == m_pPipeLine)
		return;

	m_pPipeLine->Set_Transform(eTransformState, TransformStateMatrix);
}

_matrix CGameInstance::Get_TransformMatrix(CPipeLine::D3DTRANSFORMSTATE eTransformState)
{
	if (nullptr == m_pPipeLine)
		return _matrix();

	return m_pPipeLine->Get_TransformMatrix(eTransformState);
}

_float4x4* CGameInstance::Get_TransformFloat4x4(CPipeLine::D3DTRANSFORMSTATE eTransformState)
{
	if (nullptr == m_pPipeLine)
		return nullptr;

	return m_pPipeLine->Get_TransformFloat4x4(eTransformState);
}

_matrix CGameInstance::Get_TransformMatrix_Inverse(CPipeLine::D3DTRANSFORMSTATE eTransformState)
{
	if (nullptr == m_pPipeLine)
		return _matrix();

	return m_pPipeLine->Get_TransformMatrix_Inverse(eTransformState);
}

_float4x4* CGameInstance::Get_TransformFloat4x4_Inverse(CPipeLine::D3DTRANSFORMSTATE eTransformState)
{
	if (nullptr == m_pPipeLine)
		return nullptr;

	return m_pPipeLine->Get_TransformFloat4x4_Inverse(eTransformState);
}

_float4* CGameInstance::Get_CamPosition()
{
	if (nullptr == m_pPipeLine)
		return nullptr;

	return m_pPipeLine->Get_CamPosition();
}

HRESULT CGameInstance::Get_MouseRay(ID3D11DeviceContext* pContext, HWND hWnd, _fmatrix PickingWorldMatrix_Inverse, _Inout_ _float4* vRayPos, _Inout_ _float4* vRayDir)
{
	if (nullptr == m_pCalculator)
		return E_FAIL;

	return m_pCalculator->Get_MouseRay(pContext, hWnd, PickingWorldMatrix_Inverse, vRayPos, vRayDir);
}

_bool CGameInstance::IsMouseInClient(ID3D11DeviceContext* pContext, HWND hWnd)
{
	if (nullptr == m_pCalculator)
		return false;
	return m_pCalculator->IsMouseInClient(pContext, hWnd);
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
	CInput_Device::GetInstance()->DestroyInstance();
	CGraphic_Device::GetInstance()->DestroyInstance();
}

void CGameInstance::Free()
{
	Safe_Release(m_pCalculator);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pGraphic_Device);
}
