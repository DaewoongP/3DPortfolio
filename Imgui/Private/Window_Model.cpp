#include "..\Public\Window_Model.h"
#include "DummyObject.h"

CWindow_Model::CWindow_Model()
{
}

HRESULT CWindow_Model::Initialize(void* pArg)
{
	m_vWindowPos = ImVec2(-1270, g_iWinSizeY);
	m_vWindowSize = ImVec2(1260, 300);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	m_ModelPrototypes = pGameInstance->Find_PrototypesBySubTag(LEVEL_TOOL, TEXT("Model"));
	Safe_Release(pGameInstance);

	for (auto& Pair : m_ModelPrototypes)
	{
		_char* pName = New _char[MAX_STR];
		WCharToChar(Pair.first, pName);
		m_MeshItems.push_back(pName);
	}

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

	Open_Dialog();
	
	End();
}

HRESULT CWindow_Model::Select_MeshFiles()
{

	Checkbox("Pick Meshes", &m_bPickMeshes);

	if (ImGui::ListBox("Prototypes", &m_iCur_Mesh, m_MeshItems.data(), (_int)m_MeshItems.size(), 4))
		int a = 1;
	return S_OK;
}

HRESULT CWindow_Model::Open_Dialog()
{
	if (Button("Open File Dialog"))
		IMFILE->OpenDialog("ChooseFileDlgKey", "Choose File", ".png, .dds, .fbx", ".", m_iMaxSelection);

	// display
	if (IMFILE->Display("ChooseFileDlgKey"))
	{
		// action if OK
		if (IMFILE->IsOk())
		{
			m_SelectionMap = IMFILE->GetSelection();
			string filePathName = IMFILE->GetFilePathName();
			string filePath = IMFILE->GetCurrentPath();

			wstring fPath;
			fPath.assign(filePathName.begin(), filePathName.end());
			//const _tchar* test = fPath.c_str();
		}

		// close
		IMFILE->Close();
	}
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
		_float4 vPickPos = _float4(0.f, 0.f, 0.f, 1.f);
		if (FAILED(m_pTerrain->PickingOnTerrain(&vPickPos)))
			return E_FAIL;

		wstring strName = TEXT("GameObject_DummyObject");
		wstring strNum = to_wstring(m_iDummyNum++);
		strName = strName + strNum;
		
		CDummyObject::OBJECTDESC ObjectDesc;
		CharToWChar(m_MeshItems[m_iCur_Mesh], ObjectDesc.pModelPrototypeTag);
		ObjectDesc.vPosition = vPickPos;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TOOL,
			TEXT("Prototype_GameObject_DummyObject"), TEXT("Layer_Tool"), strName.c_str(), &ObjectDesc)))
		{
			MSG_BOX("Failed Add GameObject DummyObject");
			return E_FAIL;
		}
	}
	ImGui::TextColored(ImVec4(0.f, 1.f, 1.f, 1.f),to_string(m_iDummyNum).c_str());
	return S_OK;
}

CWindow_Model* CWindow_Model::Create(void* pArg)
{
	CWindow_Model* pInstance = New CWindow_Model();

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
	for (auto& iter : m_MeshItems)
		Safe_Delete(iter);
	m_MeshItems.clear();
}
