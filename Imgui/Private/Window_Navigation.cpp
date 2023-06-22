#include "..\Public\Window_Navigation.h"
#include "Terrain.h"
#include "ImWindow_Manager.h"
#include "Bounding_Sphere.h"

CWindow_Navigation::CWindow_Navigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	ZeroMemory(m_vCell, sizeof(_float3) * CCell::POINT_END);
}

HRESULT CWindow_Navigation::Initialize(void* pArg)
{
	m_pTerrain = static_cast<CTerrain*>(m_pGameInstance->Find_GameObject(LEVEL_TOOL, TEXT("Layer_Tool"), TEXT("GameObject_Terrain")));

	if (nullptr == m_pTerrain)
		return E_FAIL;

	Safe_AddRef(m_pTerrain);

	m_PickCellBuffers.clear();

	return S_OK;
}

void CWindow_Navigation::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	ImGui::Begin("Navigation", nullptr, m_WindowFlag);

	if (true == m_bCellModifyMode)
	{
		ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Cell Modify Mode");
		ImGui::SameLine();
		if (ImGui::Button("Modify End", ImVec2(90, 20)))
			m_bCellModifyMode = false;
	}
	
	Pick_Navigation(dTimeDelta);

	CurrentNavigationPosition();

	Navigation_List();

	Delete_Cell();

	if (false == m_bCellModifyMode)
		NavigationSaveLoad();

	ImGui::End();
}

HRESULT CWindow_Navigation::Pick_Navigation(_double dTimeDelta)
{
	if (false == m_bCellModifyMode && ImGui::Checkbox("Pick Cell", &m_bPickNavigation))
	{
		// 셀 피킹 모드가 되면 다른곳에있는 피킹을 종료 해줌.
		if (true == m_bPickNavigation)
			MODELWINDOW->Set_Picking(false);
	}

	if (m_pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_DOWN) &&
		(true == m_bPickNavigation || true == m_bCellModifyMode) &&
		m_pGameInstance->IsMouseInClient(m_pContext, g_hWnd))
	{
		_float4 vPickPos = _float4(0.f, 0.f, 0.f, 1.f);

		if (true == m_pTerrain->IsPickingOnCell(&vPickPos, m_PickCellIndex, m_PickCellBuffers))
		{
			m_bCellModifyMode = true;
			
		}
		else
		{
			Pick_Terrain();
		}

	}

	if (m_pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_PRESSING) &&
		true == m_bCellModifyMode &&
		m_pGameInstance->IsMouseInClient(m_pContext, g_hWnd) &&
		0 < m_PickCellBuffers.size())
	{
		_long dwMouseMove = { 0 };
		dwMouseMove = m_pGameInstance->Get_DIMouseMove(CInput_Device::DIMM_Y);

		if (dwMouseMove)
		{
			for (auto& Buffer : m_PickCellBuffers)
			{
				// 셀의 버텍스 버퍼들 (현재 3개)
				vector<_float3> VertexPositions = Buffer.second->Get_VertexPositions();

				// Buffer.first == 버퍼의 인덱스 (POINT A,B,C)
				if (VertexPositions.size() < Buffer.first)
					return E_FAIL;

				VertexPositions[Buffer.first].y -= _float(dwMouseMove * dTimeDelta);
				
				Buffer.second->Begin(VertexPositions.data());
				Buffer.second->End();

				// 움직인 셀의 버텍스를 갱신.
				for (auto& Index : m_PickCellIndex)
				{
					m_Cells[Index][Buffer.first] = VertexPositions[Buffer.first];
				}
				
				m_vCell[Buffer.first] = VertexPositions[Buffer.first];

				m_iCurrentListBoxIndex = m_PickCellIndex[0];

				memcpy(m_vCell, m_Cells[m_iCurrentListBoxIndex], sizeof(_float3) * CCell::POINT_END);
			}

		}
	}

	if (m_pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_UP))
	{
		// 현재 움직이고 있던 버퍼 포지션 클리어.
		m_PickCellBuffers.clear();
		m_PickCellIndex.clear();
	}

	return S_OK;
}

HRESULT CWindow_Navigation::Pick_Terrain()
{
	if (true == m_bCellModifyMode)
		return S_OK;

	_float4 vPickPos = _float4(0.f, 0.f, 0.f, 1.f);

	if (FAILED(m_pTerrain->PickingOnTerrain(&vPickPos)))
		return E_FAIL;

	// 컨트롤을 누르고 피킹하면 반올림하여 처리
	if (m_pGameInstance->Get_DIKeyState(DIK_LCONTROL))
	{
		vPickPos.x = roundf(vPickPos.x);
		vPickPos.y = roundf(vPickPos.y);
		vPickPos.z = roundf(vPickPos.z);
	}
	// 처음 피킹하면 현재 Input에있는 모든값을 0으로 초기화 해줌.
	if (CCell::POINT_A == m_iCurrentPickIndex)
	{
		ZeroMemory(m_vCell, sizeof(_float3) * CCell::POINT_END);
	}
	// 피킹처리
	if (CCell::POINT_END > m_iCurrentPickIndex)
	{
		memcpy(&m_vCell[m_iCurrentPickIndex++], &vPickPos, sizeof _float3);
	}
	// 3번의 피킹이 끝나면 벡터컨테이너에 값을 넣고 피킹 인덱스 초기화
	if (CCell::POINT_END == m_iCurrentPickIndex)
	{
		_char* pIndex = New _char[6];
		_itoa_s((_uint)m_Cells.size(), pIndex, sizeof(6), 10);

		m_CellIndices.push_back(pIndex);

		_float3* pPoints = New _float3[CCell::POINT_END];
		CCWSort_Cell(m_vCell);
		memcpy(pPoints, m_vCell, sizeof(_float3) * CCell::POINT_END);

		m_Cells.push_back(pPoints);

		m_iCurrentPickIndex = CCell::POINT_A;

		Remake_Cells();
	}

	return S_OK;
}

HRESULT CWindow_Navigation::CurrentNavigationPosition()
{
	SetNextItemWidth(150.f);
	if (ImGui::InputFloat3("Cell A", reinterpret_cast<_float*>(&m_vCell[0]), "%.2f") && m_bCellModifyMode)
	{
		m_Cells[m_iCurrentListBoxIndex][0] = m_vCell[0];
		CCWSort_Cell(m_Cells[m_iCurrentListBoxIndex]);
		Remake_Cells();
	}
	SetNextItemWidth(150.f);
	if (ImGui::InputFloat3("Cell B", reinterpret_cast<_float*>(&m_vCell[1]), "%.2f") && m_bCellModifyMode)
	{
		m_Cells[m_iCurrentListBoxIndex][1] = m_vCell[1];
		CCWSort_Cell(m_Cells[m_iCurrentListBoxIndex]);
		Remake_Cells();
	}
	SetNextItemWidth(150.f);
	if (ImGui::InputFloat3("Cell C", reinterpret_cast<_float*>(&m_vCell[2]), "%.2f") && m_bCellModifyMode)
	{
		m_Cells[m_iCurrentListBoxIndex][2] = m_vCell[2];
		CCWSort_Cell(m_Cells[m_iCurrentListBoxIndex]);
		Remake_Cells();
	}

	return S_OK;
}

HRESULT CWindow_Navigation::Navigation_List()
{
	SetNextItemWidth(150.f);
	if (ImGui::ListBox("Cell Index", &m_iCurrentListBoxIndex, m_CellIndices.data(), (_int)m_CellIndices.size(), 4))
	{
		CCamera_Free* pCam = dynamic_cast<CCamera_Free*>(m_pGameInstance->Find_GameObject(LEVEL_TOOL, TEXT("Layer_Tool"), TEXT("GameObject_Camera_Free")));

		if (nullptr == pCam)
			return E_FAIL;

		m_bCellModifyMode = true;
		ZeroMemory(m_vCell, sizeof(_float3) * CCell::POINT_END);
		memcpy(m_vCell, m_Cells[m_iCurrentListBoxIndex], sizeof(_float3) * CCell::POINT_END);

		// 셀의 중심값을 바라보게 카메라 처리
		_vector vTransform;
		ZEROMEM(&vTransform);
		for (_uint i = 0; i < CCell::POINT_END; ++i)
		{
			vTransform += XMLoadFloat3(&m_vCell[i]);
		}
		vTransform /= 3.f;
		_float4 vCamPos, vPos;
		XMStoreFloat4(&vPos, vTransform);
		vPos.w = 1.f;
		vCamPos = vPos;
		vCamPos.y += 5.f;
		vCamPos.z -= 5.f;
		pCam->Set_CameraView(vCamPos, vPos, _float4(0.f, 1.f, 0.f, 0.f));
	}

	return S_OK;
}

HRESULT CWindow_Navigation::Remake_Cells()
{
	m_pTerrain->RemakeCells(m_Cells);

	return S_OK;
}

HRESULT CWindow_Navigation::Delete_Cell()
{
	if (m_bCellModifyMode && ImGui::Button("Delete Cell", ImVec2(100, 20)) && m_Cells.size() > 0)
	{
		auto iter = m_Cells.begin();
		Safe_Delete(m_Cells[m_iCurrentListBoxIndex]);
		m_Cells.erase(iter + m_iCurrentListBoxIndex);

		auto iterName = m_CellIndices.begin();
		Safe_Delete_Array(m_CellIndices[m_iCurrentListBoxIndex]);
		m_CellIndices.erase(iterName + m_iCurrentListBoxIndex);

		if (0 != m_iCurrentListBoxIndex)
			--m_iCurrentListBoxIndex;

		if (m_Cells.size() > 0)
			memcpy(m_vCell, m_Cells[m_iCurrentListBoxIndex], sizeof(_float3) * CCell::POINT_END);

		m_pTerrain->RemakeCells(m_Cells);
	}

	return S_OK;
}

HRESULT CWindow_Navigation::NavigationSaveLoad()
{
	ImGui::PushID(0);
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(2 / 7.0f, 0.6f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(2 / 7.0f, 0.7f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(2 / 7.0f, 0.8f, 0.8f));
	if (ImGui::Button("Navigation Save"))
	{
		// Pick Disable
		m_bPickNavigation = false;
		MODELWINDOW->Set_Picking(false);

		IMFILE->OpenDialog("SaveNavigationDialog", "Choose Folder", ".Navi", "Navigation.Navi");
	}
	ImGui::PopStyleColor(3);
	ImGui::PopID();

	NavigationSaveButton();

	ImGui::SameLine();
	ImGui::PushID(0);
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.6f, 1.0f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.7f, 1.0f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.8f, 1.0f, 0.8f));
	if (ImGui::Button("Navigation Load"))
	{
		m_bPickNavigation = false;
		MODELWINDOW->Set_Picking(false);

		IMFILE->OpenDialog("LoadNavigationDialog", "Choose File", ".Navi", ".");
	}
	ImGui::PopStyleColor(3);
	ImGui::PopID();

	NavigationLoadButton();

	return S_OK;
}

HRESULT CWindow_Navigation::NavigationSaveButton()
{
	// display
	if (IMFILE->Display("SaveNavigationDialog"))
	{
		// action if OK
		if (IMFILE->IsOk())
		{
			string filePath = IMFILE->GetFilePathName();
			_tchar wszPath[MAX_PATH] = TEXT("");
			CharToWChar(filePath.c_str(), wszPath);
			if (FAILED(NavigationWrite_File(wszPath)))
				MSG_BOX("Failed File Write");
		}

		// close
		IMFILE->Close();
	}
	return S_OK;
}

HRESULT CWindow_Navigation::NavigationWrite_File(const _tchar* pPath)
{
	HANDLE hFile = CreateFile(pPath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong	dwByte = 0;
	
	_uint iSize = (_uint)m_Cells.size();
	WriteFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);
	for (auto pCell : m_Cells)
		WriteFile(hFile, pCell, sizeof(_float3) * CCell::POINT_END, &dwByte, nullptr);

	
	CloseHandle(hFile);

	MSG_BOX("File Save Success");
	return S_OK;
}

HRESULT CWindow_Navigation::NavigationLoadButton()
{
	// display
	if (IMFILE->Display("LoadNavigationDialog"))
	{
		// action if OK
		if (IMFILE->IsOk())
		{
			map<string, string> strMap = IMFILE->GetSelection();
			string filePathName = IMFILE->GetFilePathName();
			_tchar wszName[MAX_PATH] = TEXT("");
			CharToWChar(filePathName.c_str(), wszName);
			if (FAILED(NavigationRead_File(wszName)))
				MSG_BOX("Failed File Read");
		}

		// close
		IMFILE->Close();
	}
	return S_OK;
}

HRESULT CWindow_Navigation::NavigationRead_File(const _tchar* pFileName)
{
	HANDLE hFile = CreateFile(pFileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong	dwByte = 0;
	_uint iSize = 0;
	ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iSize; ++i)
	{
		_float3* pCell = New _float3[CCell::POINT_END];
		ReadFile(hFile, pCell, sizeof(_float3) * CCell::POINT_END, &dwByte, nullptr);

		_char* pIndex = New _char[6];
		_itoa_s((_uint)m_Cells.size(), pIndex, sizeof(6), 10);
		m_CellIndices.push_back(pIndex);

		m_Cells.push_back(pCell);

		m_iCurrentPickIndex = 0;
		Remake_Cells();
	}
	memcpy(&m_vCell, m_Cells.front(), sizeof(_float3) * CCell::POINT_END);
	CloseHandle(hFile);

	MSG_BOX("File Load Success");
	return S_OK;
}

void CWindow_Navigation::CCWSort_Cell(_float3* pPoints)
{
	_vector vAB, vAC, vCross;
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	vAB = XMLoadFloat3(&pPoints[CCell::POINT_B]) - XMLoadFloat3(&pPoints[CCell::POINT_A]);
	vAC = XMLoadFloat3(&pPoints[CCell::POINT_C]) - XMLoadFloat3(&pPoints[CCell::POINT_A]);

	vCross = XMVector3Normalize(XMVector3Cross(vAB, vAC));
	
	// not CCW
	if (0 > XMVectorGetX(XMVector3Dot(vUp, vCross)))
	{
		_float3 vSour = pPoints[CCell::POINT_B];
		pPoints[CCell::POINT_B] = pPoints[CCell::POINT_C];
		pPoints[CCell::POINT_C] = vSour;
	}
}

CWindow_Navigation* CWindow_Navigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CWindow_Navigation* pInstance = New CWindow_Navigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CWindow_Navigation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWindow_Navigation::Free()
{
	__super::Free();

	for (auto& pCell : m_Cells)
		Safe_Delete(pCell);
	m_Cells.clear();

	Safe_Release(m_pTerrain);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	for (auto& pIndex : m_CellIndices)
	{
		Safe_Delete_Array(pIndex);
	}
}
