#include "..\Public\Window_Model.h"
#include "AnimModel.h"
#include "NonAnimModel.h"
#include "ImWindow_Manager.h"
#include "Window_Object.h"

CWindow_Model::CWindow_Model(ID3D11DeviceContext* pContext)
	: m_pContext(pContext)
{
	Safe_AddRef(m_pContext);
	ZEROMEM(&m_vScale);
	ZEROMEM(&m_vRotation);
	ZEROMEM(&m_vTransform);
	m_vTransform.w = 1.f;
}

HRESULT CWindow_Model::Initialize(void* pArg)
{
	m_vWindowPos = ImVec2(-1270, g_iWinSizeY);
	m_vWindowSize = ImVec2(400, 300);

	Initialize_Transforms();

	m_NonAnimModelPrototypes = m_pGameInstance->Find_PrototypesBySubTag(LEVEL_TOOL, TEXT("Component_NonAnimModel"));
	m_AnimModelPrototypes = m_pGameInstance->Find_PrototypesBySubTag(LEVEL_TOOL, TEXT("Component_AnimModel"));

	for (auto& Pair : m_NonAnimModelPrototypes)
	{
		_char pName[MAX_STR] = "";
		WCharToChar(Pair.first, pName);
		_char* szSubName;
		_char* pItemName = New _char[MAX_STR];
		strtok_s(pName, "_", &szSubName);
		strtok_s(nullptr, "_", &szSubName);
		strtok_s(nullptr, "_", &szSubName);
		strcpy_s(pItemName, MAX_STR, szSubName);
		
		m_NonAnimModelItems.push_back(pItemName);
	}

	sort(m_NonAnimModelItems.begin(), m_NonAnimModelItems.end(), [](auto& Sour, auto& Dest) {
		string strSour = Sour;
		string strDest = Dest;
		if (strSour < strDest)
			return true;
		else
			return false;
		});

	for (auto& Pair : m_AnimModelPrototypes)
	{
		_char pName[MAX_STR] = "";
		WCharToChar(Pair.first, pName);
		_char* szSubName;
		_char* pItemName = New _char[MAX_STR];
		strtok_s(pName, "_", &szSubName);
		strtok_s(nullptr, "_", &szSubName);
		strtok_s(nullptr, "_", &szSubName);
		strcpy_s(pItemName, MAX_STR, szSubName);

		m_AnimModelItems.push_back(pItemName);
	}

	sort(m_AnimModelItems.begin(), m_AnimModelItems.end(), [](auto& Sour, auto& Dest) {
		string strSour = Sour;
		string strDest = Dest;
		if (strSour < strDest)
			return true;
		else
			return false;
		});

	m_pTerrain = static_cast<CTerrain*>(m_pGameInstance->Find_GameObject(LEVEL_TOOL, TEXT("Layer_Tool"), TEXT("GameObject_Terrain")));
	if (nullptr == m_pTerrain)
		return E_FAIL;
	Safe_AddRef(m_pTerrain);

	MakeTag(NONANIM);
	return S_OK;
}

void CWindow_Model::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
	Begin("Model", nullptr, m_WindowFlag);

	MakeObject(dTimeDelta);

	Select_ModelFiles();

	Setting_Transform();

	SaveLoad();

	End();
}

HRESULT CWindow_Model::Select_ModelFiles()
{
	Checkbox("Pick Meshes", &m_bPickMeshes);
	SameLine();
	SetNextItemWidth(200.f);

	if (RadioButton("Non Anim", &m_iCurRadio, 0))
	{
		m_iCur_Mesh = 0;
		MakeTag(NONANIM);
	}
		
	SameLine();

	if (RadioButton("Anim", &m_iCurRadio, 1))
	{
		m_iCur_Mesh = 0;
		MakeTag(ANIM);
	}
		

	if (NONANIM == m_iCurRadio)
	{
		if (ListBox("Models", &m_iCur_Mesh, m_NonAnimModelItems.data(), (_int)m_NonAnimModelItems.size(), 7))
		{
			MakeTag(NONANIM);
		}
	}
	else if (ANIM == m_iCurRadio)
	{
		if (ListBox("Models", &m_iCur_Mesh, m_AnimModelItems.data(), (_int)m_AnimModelItems.size(), 7))
		{
			MakeTag(ANIM);
		}
	}

	return S_OK;
}

HRESULT CWindow_Model::Setting_Transform()
{
	SetNextItemWidth(300.f);
	if (InputFloat3("Scale", reinterpret_cast<_float*>(&m_vScale)))
	{
		if (m_vScale.x <= 0.001f)
			m_vScale.x = 0.1f;
		if (m_vScale.y <= 0.001f)
			m_vScale.y = 0.1f;
		if (m_vScale.z <= 0.001f)
			m_vScale.z = 0.1f;
		CDummy* pDummy = dynamic_cast<CDummy*>(m_pGameInstance->Get_LastGameObject());
		pDummy->Get_TransformCom()->Set_Scale(m_vScale);
		pDummy->Set_PreToolScale(m_vScale);
	}
	SetNextItemWidth(300.f);
	if (InputFloat3("Rotation", reinterpret_cast<_float*>(&m_vRotation)))
	{
		CDummy* pDummy = dynamic_cast<CDummy*>(m_pGameInstance->Get_LastGameObject());
		pDummy->Get_TransformCom()->Rotation(m_vRotation);
		pDummy->Set_PreToolRotation(m_vRotation);
	}
	SetNextItemWidth(300.f);
	if (InputFloat3("Transform", reinterpret_cast<_float*>(&m_vTransform)))
	{
		CDummy* pDummy = dynamic_cast<CDummy*>(m_pGameInstance->Get_LastGameObject());
		pDummy->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_vTransform));
		pDummy->Set_PreToolTransform(m_vTransform);
	}
	return S_OK;
}

HRESULT CWindow_Model::MakeObject(_double dTimeDelta)
{
	ImGui::InputText("Object Tag", m_szObjectName, MAX_STR);

	if (m_pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_DOWN) && 
		m_bPickMeshes &&
		CGameInstance::GetInstance()->IsMouseInClient(m_pContext, g_hWnd))
	{
		_tchar wszName[MAX_STR] = TEXT("");
		CharToWChar(m_szObjectName, wszName);

		_float4 vPickPos = _float4(0.f, 0.f, 0.f, 1.f);
		if (FAILED(m_pTerrain->PickingOnTerrain(&vPickPos)))
			return E_FAIL;
		
		if (m_pGameInstance->Get_DIKeyState(DIK_LCONTROL))
		{
			vPickPos.x = roundf(vPickPos.x);
			vPickPos.y = roundf(vPickPos.y);
			vPickPos.z = roundf(vPickPos.z);
		}

		Initialize_Transforms();

		if (NONANIM == m_iCurRadio)
		{
			MakeNonAnimModel(wszName, vPickPos);
		}
		else if (ANIM == m_iCurRadio)
		{
			MakeAnimModel(wszName, vPickPos);
		}

		Initialize_DummyTransforms();
	}
	return S_OK;
}

HRESULT CWindow_Model::Initialize_Transforms()
{
	m_vScale = _float3(1.f, 1.f, 1.f);
	ZEROMEM(&m_vRotation);
	m_vTransform = _float4(0.f, 0.f, 0.f, 1.f);

	return S_OK;
}

HRESULT CWindow_Model::Initialize_DummyTransforms()
{
	CDummy* pDummy = dynamic_cast<CDummy*>(m_pGameInstance->Get_LastGameObject());
	if (nullptr == pDummy)
		return E_FAIL;

	pDummy->Set_PreToolScale(m_vScale);
	pDummy->Set_PreToolRotation(m_vRotation);
	pDummy->Set_PreToolTransform(m_vTransform);
	return S_OK;
}

HRESULT CWindow_Model::MakeNonAnimModel(const _tchar* pName, _float4 vPickPos)
{
	CAnimModel::OBJECTDESC ObjectDesc;
	_char szProtoName[MAX_STR] = "Prototype_Component_NonAnimModel_";
	strcat_s(szProtoName, MAX_STR, m_NonAnimModelItems[m_iCur_Mesh]);
	CharToWChar(szProtoName, ObjectDesc.pModelPrototypeTag);

	ObjectDesc.vPosition = vPickPos;
	m_vTransform = vPickPos;

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TOOL,
		TEXT("Prototype_GameObject_NonAnimModel"), TEXT("Layer_Tool"), pName, &ObjectDesc)))
	{
		MSG_BOX("Failed Add GameObject NonAnimModel");
		return E_FAIL;
	}

	OBJECTWINDOW->Set_Object(m_pGameInstance->Get_LastGameObject());

	m_iCur_Mesh_Index++;
	MakeTag(NONANIM);
	return S_OK;
}

HRESULT CWindow_Model::MakeAnimModel(const _tchar* pName, _float4 vPickPos)
{
	CAnimModel::OBJECTDESC ObjectDesc;
	_char szProtoName[MAX_STR] = "Prototype_Component_AnimModel_";
	strcat_s(szProtoName, MAX_STR, m_AnimModelItems[m_iCur_Mesh]);
	CharToWChar(szProtoName, ObjectDesc.pModelPrototypeTag);

	ObjectDesc.vPosition = vPickPos;
	m_vTransform = vPickPos;

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TOOL,
		TEXT("Prototype_GameObject_AnimModel"), TEXT("Layer_Tool"), pName, &ObjectDesc)))
	{
		MSG_BOX("Failed Add GameObject AnimModel");
		return E_FAIL;
	}

	OBJECTWINDOW->Set_Object(m_pGameInstance->Get_LastGameObject());

	m_iCur_Mesh_Index++;
	MakeTag(ANIM);
	return S_OK;
}

HRESULT CWindow_Model::MakeTag(RADIO eType)
{
	if (NONANIM == eType)
	{
		strcpy_s(m_szObjectName, MAX_STR, "GameObject_NonAnimModel_");
		if (m_NonAnimModelItems.size() <= m_iCur_Mesh)
			return E_FAIL;
		strcat_s(m_szObjectName, MAX_STR, m_NonAnimModelItems[m_iCur_Mesh]);
	}
	else if (ANIM == eType)
	{
		strcpy_s(m_szObjectName, MAX_STR, "GameObject_AnimModel_");
		if (m_AnimModelItems.size() <= m_iCur_Mesh)
			return E_FAIL;
		strcat_s(m_szObjectName, MAX_STR, m_AnimModelItems[m_iCur_Mesh]);
	}
	
	_char szIndex[MAX_STR] = "";
	_itoa_s(m_iCur_Mesh_Index, szIndex, MAX_STR, 10);
	strcat_s(m_szObjectName, MAX_STR, szIndex);

	return S_OK;
}

HRESULT CWindow_Model::SaveLoad()
{
	ImGui::PushID(0);
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(2 / 7.0f, 0.6f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(2 / 7.0f, 0.7f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(2 / 7.0f, 0.8f, 0.8f));
	if (ImGui::Button("All Save"))
	{
		m_bPickMeshes = false;
		IMFILE->OpenDialog("SaveDialog", "Choose Folder", nullptr, ".");
	}
	ImGui::PopStyleColor(3);
	ImGui::PopID();

	SaveButton();

	ImGui::SameLine();
	ImGui::PushID(0);
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.6f, 1.0f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.7f, 1.0f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.8f, 1.0f, 0.8f));
	if (ImGui::Button("All Load"))
	{
		m_bPickMeshes = false;
		IMFILE->OpenDialog("LoadDialog", "Choose File", ".test", ".");
	}
	ImGui::PopStyleColor(3);
	ImGui::PopID();

	LoadButton();

	return S_OK;
}

HRESULT CWindow_Model::SaveButton()
{
	// display
	if (IMFILE->Display("SaveDialog"))
	{
		// action if OK
		if (IMFILE->IsOk())
		{
			string filePath = IMFILE->GetCurrentPath();
			string fileName = "\\ObjectData.test";
			filePath += fileName;
			_tchar wszPath[MAX_PATH] = TEXT("");
			CharToWChar(filePath.c_str(), wszPath);
			Write_File(wszPath);
		}

		// close
		IMFILE->Close();
	}
	return S_OK;
}

HRESULT CWindow_Model::Write_File(const _tchar* pPath)
{
	HANDLE hFile = CreateFile(pPath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong	dwByte = 0;
	_ulong	dwStrByte = 0;
	vector<class CGameObject*> Objects = OBJECTWINDOW->Get_Objects();

	for (auto& pObject : Objects)
	{
		// Object Tag
		dwStrByte = sizeof(_tchar) * (lstrlen(pObject->Get_Tag()) + 1);
		WriteFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
		WriteFile(hFile, pObject->Get_Tag(), dwStrByte, &dwByte, nullptr);
		CDummy* pDummy = static_cast<CDummy*>(pObject);
		

		// Object State
		WriteFile(hFile, &(pDummy->Get_PreToolScale()), sizeof(_float3), &dwByte, nullptr);
		WriteFile(hFile, &(pDummy->Get_PreToolRotation()), sizeof(_float3), &dwByte, nullptr);
		WriteFile(hFile, &(pDummy->Get_PreToolTransform()), sizeof(_float4), &dwByte, nullptr);
	}

	CloseHandle(hFile);
	return S_OK;
}

HRESULT CWindow_Model::LoadButton()
{
	// display
	if (IMFILE->Display("LoadDialog"))
	{
		// action if OK
		if (IMFILE->IsOk())
		{
			map<string, string> strMap = IMFILE->GetSelection();
			string filePathName = IMFILE->GetFilePathName();
			string filePath = IMFILE->GetCurrentPath();

			wstring fPath;
			fPath.assign(filePathName.begin(), filePathName.end());
		}

		// close
		IMFILE->Close();
	}
	return S_OK;
}

CWindow_Model* CWindow_Model::Create(ID3D11DeviceContext* pContext, void* pArg)
{
	CWindow_Model* pInstance = New CWindow_Model(pContext);

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

	for (auto& iter : m_NonAnimModelItems)
		Safe_Delete(iter);
	m_NonAnimModelItems.clear();

	for (auto& iter : m_AnimModelItems)
		Safe_Delete(iter);
	m_AnimModelItems.clear();

	Safe_Release(m_pContext);
}
