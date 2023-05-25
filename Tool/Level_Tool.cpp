#include "pch.h"
#include "Level_Tool.h"
#include "GameInstance.h"
#include "ToolInstance.h"

CLevel_Tool::CLevel_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Tool::Initialize()
{
	FAILED_CHECK_RETURN(__super::Initialize(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_For_Layer_Terrain(TEXT("Layer_Terrain")), E_FAIL);
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

HRESULT CLevel_Tool::Ready_For_Layer_Terrain(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* For.GameObject_Terrain */
	if (FAILED(pGameInstance->Add_GameObject(static_cast<_uint>(LEVELID::LEVEL_TOOL),
		TEXT("Prototype_GameObject_Terrain"), TEXT("GameObject_Terrain"))))
	{
		AfxMessageBox(TEXT("Failed Add GameObject CTerrain"));
		return E_FAIL;
	}
	/* For.GameObject_Camera */
	if (FAILED(pGameInstance->Add_GameObject(static_cast<_uint>(LEVELID::LEVEL_TOOL),
		TEXT("Prototype_GameObject_Camera"), TEXT("GameObject_Camera"))))
	{
		AfxMessageBox(TEXT("Failed Add GameObject CCamera"));
		return E_FAIL;
	}
	/* For.GameObject_Axis */
	if (FAILED(pGameInstance->Add_GameObject(static_cast<_uint>(LEVELID::LEVEL_TOOL),
		TEXT("Prototype_GameObject_Axis"), TEXT("GameObject_Axis"))))
	{
		AfxMessageBox(TEXT("Failed Add GameObject CAxis"));
		return E_FAIL;
	}
	/* For.GameObject_DummyObject */
	if (FAILED(pGameInstance->Add_GameObject(static_cast<_uint>(LEVELID::LEVEL_TOOL),
		TEXT("Prototype_GameObject_DummyObject"), TEXT("GameObject_DummyObject"))))
	{
		AfxMessageBox(TEXT("Failed Add GameObject CDummyObject"));
		return E_FAIL;
	}

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
}
