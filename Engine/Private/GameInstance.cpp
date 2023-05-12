#include "..\Public\GameInstance.h"
#include "Graphic_Device.h"
#include "Level_Manager.h"
#include "Object_Manager.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
	: m_pGraphic_Device{ CGraphic_Device::GetInstance() }
	, m_pLevel_Manager{ CLevel_Manager::GetInstance() }
	, m_pObject_Manager{ CObject_Manager::GetInstance() }
{
	Safe_AddRef(m_pGraphic_Device);
	Safe_AddRef(m_pLevel_Manager);
	Safe_AddRef(m_pObject_Manager);
}

HRESULT CGameInstance::Initialize_Engine(_uint iNumLevels, const GRAPHICDESC& GraphicDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext)
{
	NULL_CHECK_RETURN(m_pGraphic_Device, E_FAIL);
	FAILED_CHECK_RETURN_MSG(m_pGraphic_Device->Ready_Graphic_Device(GraphicDesc.hWnd, GraphicDesc.eWinMode, GraphicDesc.iViewportSizeX, GraphicDesc.iViewportSizeY, ppDevice, ppContext), E_FAIL,
		L"Failed Ready_Graphic_Device");

	FAILED_CHECK_RETURN_MSG(m_pObject_Manager->Reserve_Containers(iNumLevels), E_FAIL,
		L"Failed Reserve_Containers");
	return S_OK;
}

void CGameInstance::Tick_Engine(_double dTimeDelta)
{
	if (nullptr == m_pLevel_Manager)
		return;

	m_pLevel_Manager->Tick(dTimeDelta);
	m_pObject_Manager->Tick(dTimeDelta);


	m_pObject_Manager->Late_Tick(dTimeDelta);
}

void CGameInstance::Clear_LevelResources(_uint iLevelIndex)
{
	if (nullptr == m_pObject_Manager)
		return;

	m_pObject_Manager->Clear_LevelResources(iLevelIndex);
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

HRESULT CGameInstance::Open_Level(_uint iLevelIndex, CLevel* pNewLevel)
{
	NULL_CHECK_RETURN(m_pLevel_Manager, E_FAIL);
	NULL_CHECK_RETURN(m_pObject_Manager, E_FAIL);
	
	m_pObject_Manager->Set_CurLevel(iLevelIndex);

	return m_pLevel_Manager->Open_Level(iLevelIndex, pNewLevel);
}

HRESULT CGameInstance::Add_Prototype(const _tchar* pPrototypeTag, CGameObject* pPrototype)
{
	NULL_CHECK_RETURN(m_pObject_Manager, E_FAIL);
	
	return m_pObject_Manager->Add_Prototype(pPrototypeTag, pPrototype);
}

HRESULT CGameInstance::Add_GameObject(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pLayerTag, void* pArg)
{
	NULL_CHECK_RETURN(m_pObject_Manager, E_FAIL);

	return m_pObject_Manager->Add_GameObject(iLevelIndex, pPrototypeTag, pLayerTag, pArg);
}

void CGameInstance::Release_Engine()
{
	CGameInstance::GetInstance()->DestroyInstance();
	CObject_Manager::GetInstance()->DestroyInstance();
	CLevel_Manager::GetInstance()->DestroyInstance();
	CGraphic_Device::GetInstance()->DestroyInstance();
}

void CGameInstance::Free()
{
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pGraphic_Device);
}
