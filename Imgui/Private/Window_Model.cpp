#include "..\Public\Window_Model.h"
#include "AnimModel.h"
#include "NonAnimModel.h"

CWindow_Model::CWindow_Model()
{
	ZEROMEM(&m_vScale);
	ZEROMEM(&m_vRotate);
	ZEROMEM(&m_vTransform);
	m_vTransform.w = 1.f;
}

HRESULT CWindow_Model::Initialize(void* pArg)
{
	m_vWindowPos = ImVec2(-1270, g_iWinSizeY);
	m_vWindowSize = ImVec2(400, 300);

	Initialize_Transforms();

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	m_NonAnimModelPrototypes = pGameInstance->Find_PrototypesBySubTag(LEVEL_TOOL, TEXT("Component_NonAnimModel"));
	m_AnimModelPrototypes = pGameInstance->Find_PrototypesBySubTag(LEVEL_TOOL, TEXT("Component_AnimModel"));
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

	Safe_Release(pGameInstance);
	m_pTerrain = static_cast<CTerrain*>(m_pGameInstance->Find_GameObject(LEVEL_TOOL, TEXT("Layer_Tool"), TEXT("GameObject_Terrain")));
	if (nullptr == m_pTerrain)
		return E_FAIL;
	Safe_AddRef(m_pTerrain);

	strcpy_s(m_szObjectName, MAX_STR, "GameObject_NonAnimModel_");
	return S_OK;
}

void CWindow_Model::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
	Begin("Model", nullptr, m_WindowFlag);

	MakeObject(dTimeDelta);

	Select_ModelFiles();

	Setting_Transform();

	Open_Dialog();
	
	End();
}

HRESULT CWindow_Model::Select_ModelFiles()
{
	Checkbox("Pick Meshes", &m_bPickMeshes);
	SameLine();
	SetNextItemWidth(200.f);

	if (RadioButton("Non Anim", &m_iCurRadio, 0))
	{
		strcpy_s(m_szObjectName, MAX_STR, "GameObject_NonAnimModel_");
		m_iCur_Mesh = 0;
	}
		
	SameLine();

	if (RadioButton("Anim", &m_iCurRadio, 1))
	{
		strcpy_s(m_szObjectName, MAX_STR, "GameObject_AnimModel_");
		m_iCur_Mesh = 0;
	}
		

	if (NONANIM == m_iCurRadio)
		ListBox("Models", &m_iCur_Mesh, m_NonAnimModelItems.data(), (_int)m_NonAnimModelItems.size(), 4);
	else if (ANIM == m_iCurRadio)
		ListBox("Models", &m_iCur_Mesh, m_AnimModelItems.data(), (_int)m_AnimModelItems.size(), 4);

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
			/*m_SelectionMap = IMFILE->GetSelection();
			string filePathName = IMFILE->GetFilePathName();
			string filePath = IMFILE->GetCurrentPath();*/
		}

		// close
		IMFILE->Close();
	}
	return S_OK;
}

HRESULT CWindow_Model::Setting_Transform()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	SetNextItemWidth(300.f);
	if (InputFloat3("Scale", reinterpret_cast<_float*>(&m_vScale)))
	{
		if (m_vScale.x <= 0.001f)
			m_vScale.x = 0.1f;
		if (m_vScale.y <= 0.001f)
			m_vScale.y = 0.1f;
		if (m_vScale.z <= 0.001f)
			m_vScale.z = 0.1f;

		if (dynamic_cast<CAnimModel*>(m_pLastObject))
			static_cast<CAnimModel*>(m_pLastObject)->Get_TransformCom()->Set_Scale(m_vScale);
		else if (dynamic_cast<CNonAnimModel*>(m_pLastObject))
			static_cast<CNonAnimModel*>(m_pLastObject)->Get_TransformCom()->Set_Scale(m_vScale);
	}
	SetNextItemWidth(300.f);
	if (InputFloat3("Rotation", reinterpret_cast<_float*>(&m_vRotate)))
	{
		if (dynamic_cast<CAnimModel*>(m_pLastObject))
			static_cast<CAnimModel*>(m_pLastObject)->Get_TransformCom()->Rotation(m_vRotate);
		if (dynamic_cast<CNonAnimModel*>(m_pLastObject))
			static_cast<CNonAnimModel*>(m_pLastObject)->Get_TransformCom()->Rotation(m_vRotate);
	}
	SetNextItemWidth(300.f);
	if (InputFloat3("Transform", reinterpret_cast<_float*>(&m_vTransform)))
	{
		if (dynamic_cast<CAnimModel*>(m_pLastObject))
			static_cast<CAnimModel*>(m_pLastObject)->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_vTransform));
		if (dynamic_cast<CNonAnimModel*>(m_pLastObject))
			static_cast<CNonAnimModel*>(m_pLastObject)->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_vTransform));
	}

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CWindow_Model::MakeObject(_double dTimeDelta)
{
	ImGui::InputText("Object Tag", m_szObjectName, MAX_STR);

	if (m_pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_DOWN) && m_bPickMeshes)
	{
		_tchar wszName[MAX_STR] = TEXT("");
		CharToWChar(m_szObjectName, wszName);

		_float4 vPickPos = _float4(0.f, 0.f, 0.f, 1.f);
		if (FAILED(m_pTerrain->PickingOnTerrain(&vPickPos)))
			return E_FAIL;

		Initialize_Transforms();

		if (NONANIM == m_iCurRadio)
		{
			MakeNonAnimModel(wszName, vPickPos);
		}
		else if (ANIM == m_iCurRadio)
		{
			MakeAnimModel(wszName, vPickPos);
		}

	}
	return S_OK;
}

HRESULT CWindow_Model::Initialize_Transforms()
{
	m_vScale = _float3(1.f, 1.f, 1.f);
	ZEROMEM(&m_vRotate);
	m_vTransform = _float4(0.f, 0.f, 0.f, 1.f);
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

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	m_pLastObject = pGameInstance->Get_LastGameObject();
	Safe_Release(pGameInstance);

	if (nullptr == m_pLastObject)
		MSG_BOX("Failed Get LastObject");
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

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	m_pLastObject = pGameInstance->Get_LastGameObject();
	Safe_Release(pGameInstance);

	if (nullptr == m_pLastObject)
		MSG_BOX("Failed Get LastObject");
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

	for (auto& iter : m_NonAnimModelItems)
		Safe_Delete(iter);
	m_NonAnimModelItems.clear();

	for (auto& iter : m_AnimModelItems)
		Safe_Delete(iter);
	m_AnimModelItems.clear();
}
