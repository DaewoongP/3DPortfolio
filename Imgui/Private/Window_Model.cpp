#include "..\Public\Window_Model.h"

CWindow_Model::CWindow_Model()
{
}

HRESULT CWindow_Model::Initialize(void* pArg)
{
	m_vWindowPos = ImVec2(-1270, g_iWinSizeY);
	m_vWindowSize = ImVec2(1260, 300);

	m_pTerrain = static_cast<CTerrain*>(m_pGameInstance->Find_GameObject(LEVEL_TOOL, TEXT("Layer_Tool"), TEXT("GameObject_Terrain")));
	if (nullptr == m_pTerrain)
		return E_FAIL;
	Safe_AddRef(m_pTerrain);

	return S_OK;
}

void CWindow_Model::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
	Begin("Model", nullptr, m_WindowFlag);

	MakeObject(dTimeDelta);

	Select_MeshFiles();

	Setting_Transform();
	
	End();
}

HRESULT CWindow_Model::Select_MeshFiles()
{
	Checkbox("Pick Meshes", &m_bPickMeshes);
	const char* items[] = { "Apple", "Banana", "Cherry", "Kiwi", "Mango", "Orange", "Pineapple", "Strawberry", "Watermelon" };
	static int item_current = 1;
	ImGui::ListBox("listbox", &item_current, items, IM_ARRAYSIZE(items), 4); 
	return S_OK;
}

HRESULT CWindow_Model::Setting_Transform()
{
	return S_OK;
}

HRESULT CWindow_Model::MakeObject(_double dTimeDelta)
{
	if (m_pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_DOWN) && m_bPickMeshes)
	{
		_float4 vPickPos;
		if (FAILED(m_pTerrain->PickingOnTerrain(&vPickPos)))
			return E_FAIL;

		wstring strName = TEXT("GameObject_DummyObject");
		static int iNum = 0;
		wstring strNum = to_wstring(iNum);

		strName = strName + strNum;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TOOL,
			TEXT("Prototype_GameObject_DummyObject"), TEXT("Layer_Tool"), strName.c_str(), &vPickPos)))
		{
			MSG_BOX("Failed Add GameObject DummyObject");
			return E_FAIL;
		}
		++iNum;
	}

	return S_OK;
}

CWindow_Model* CWindow_Model::Create(void* pArg)
{
	CWindow_Model* pInstance = new CWindow_Model();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CWindow_Model");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWindow_Model::Free()
{
	__super::Free();
	Safe_Release(m_pTerrain);
}
