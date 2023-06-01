#include "Level_Tool.h"
#include "GameInstance.h"
#include "Axis.h"

CLevel_Tool::CLevel_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
	Safe_AddRef(m_pToolInstance);
}

HRESULT CLevel_Tool::Initialize()
{
	FAILED_CHECK_RETURN(__super::Initialize(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_For_Layer_Tool(TEXT("Layer_Tool")), E_FAIL);
	return S_OK;
}

void CLevel_Tool::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
}

HRESULT CLevel_Tool::Render()
{
	if (FAILED(__super::Render()))
	{
		MSG_BOX("Failed CLevel_Tool Rendering");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Tool::Ready_For_Layer_Tool(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* For.GameObject_Terrain */
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL,
		TEXT("Prototype_GameObject_Terrain"), pLayerTag, TEXT("GameObject_Terrain"))))
	{
		MSG_BOX("Failed Add GameObject CTerrain");
		return E_FAIL;
	}
	/* For.GameObject_Camera_Free */
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL,
		TEXT("Prototype_GameObject_Camera_Free"), pLayerTag, TEXT("GameObject_Camera_Free"))))
	{
		MSG_BOX("Failed Add GameObject CCamera_Free");
		return E_FAIL;
	}
	CAxis::AXISSTATE eState;
	eState = CAxis::AXIS_ORIGIN;
	/* For.GameObject_Axis_Origin */
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL,
		TEXT("Prototype_GameObject_Axis"), pLayerTag, TEXT("GameObject_Axis_Origin"), &eState)))
	{
		MSG_BOX("Failed Add GameObject CAxis");
		return E_FAIL;
	}
	eState = CAxis::AXIS_UI;
	/* For.GameObject_Axis_UI */
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL,
		TEXT("Prototype_GameObject_Axis"), pLayerTag, TEXT("GameObject_Axis_UI"), &eState)))
	{
		MSG_BOX("Failed Add GameObject CAxis");
		return E_FAIL;
	}
	eState = CAxis::AXIS_CENTER;
	/* For.GameObject_Axis_Center */
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL,
		TEXT("Prototype_GameObject_Axis"), pLayerTag, TEXT("GameObject_Axis_Center"), &eState)))
	{
		MSG_BOX("Failed Add GameObject CAxis");
		return E_FAIL;
	}
	
	///* For.GameObject_DummyObject */
	//if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL,
	//	TEXT("Prototype_GameObject_DummyObject"), TEXT("GameObject_DummyObject"))))
	//{
	//	MSG_BOX("Failed Add GameObject CDummyObject");
	//	return E_FAIL;
	//}
	Safe_Release(pGameInstance);
	return S_OK;
}

CLevel_Tool* CLevel_Tool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Tool* pInstance = new CLevel_Tool(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created CLevel_Tool");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Tool::Free()
{
	__super::Free();
	Safe_Release(m_pToolInstance);
}
