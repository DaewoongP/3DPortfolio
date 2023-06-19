#include "..\Public\Window_Navigation.h"
#include "Terrain.h"
#include "ImWindow_Manager.h"

CWindow_Navigation::CWindow_Navigation(ID3D11DeviceContext* pContext)
	: m_pContext(pContext)
{
	Safe_AddRef(m_pContext);
	ZeroMemory(m_Cell, sizeof(_float3) * CCell::POINT_END);
}

HRESULT CWindow_Navigation::Initialize(void* pArg)
{
	m_pTerrain = static_cast<CTerrain*>(m_pGameInstance->Find_GameObject(LEVEL_TOOL, TEXT("Layer_Tool"), TEXT("GameObject_Terrain")));

	if (nullptr == m_pTerrain)
		return E_FAIL;

	Safe_AddRef(m_pTerrain);

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
	else
	{
		Pick_Navigation();
	}

	CurrentNavigationPosition();

	Navigation_List();

	ImGui::End();
}

HRESULT CWindow_Navigation::Pick_Navigation()
{
	if (ImGui::Checkbox("Pick Cell", &m_bPickNavigation))
	{
		if (true == m_bPickNavigation)
			MODELWINDOW->Set_Picking(false);
	}
	
	if (m_pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_DOWN) &&
		m_bPickNavigation &&
		m_pGameInstance->IsMouseInClient(m_pContext, g_hWnd))
	{
		_float4 vPickPos = _float4(0.f, 0.f, 0.f, 1.f);
		if (FAILED(m_pTerrain->PickingOnTerrain(&vPickPos)))
			return E_FAIL;

		if (m_pGameInstance->Get_DIKeyState(DIK_LCONTROL))
		{
			vPickPos.x = roundf(vPickPos.x);
			vPickPos.y = roundf(vPickPos.y);
			vPickPos.z = roundf(vPickPos.z);
		}

		if (0 == m_iCurrentPickIndex)
		{
			ZeroMemory(m_Cell, sizeof(_float3) * CCell::POINT_END);
		}

		if (CCell::POINT_END > m_iCurrentPickIndex)
		{
			memcpy(&m_Cell[m_iCurrentPickIndex++], &vPickPos, sizeof _float3);
		}
		
		if (CCell::POINT_END == m_iCurrentPickIndex)
		{
			for (_uint i = 0; i < CCell::POINT_END; ++i)
			{
				m_Cells.push_back({ m_CellIndex, m_Cell[i] });
			}
			if (0 != m_Cells.size() % 3)
			{
				MSG_BOX("Cell Size Error");
				return E_FAIL;
			}
				

			_char* pIndex = new _char[6];
			_itoa_s(m_CellIndex++, pIndex, sizeof(6), 10);

			m_CellIndices.push_back(pIndex);

			m_iCurrentPickIndex = 0;

			Remake_Cells();
		}
	}

	return S_OK;
}

HRESULT CWindow_Navigation::CurrentNavigationPosition()
{
	SetNextItemWidth(150.f);
	if (ImGui::InputFloat3("Cell 0", reinterpret_cast<_float*>(&m_Cell[0]), "%.2f") && m_bCellModifyMode)
	{
		m_Cells[m_iCurrentListBoxIndex * 3].second = m_Cell[0];
		Remake_Cells();
	}
	SetNextItemWidth(150.f);
	if (ImGui::InputFloat3("Cell 1", reinterpret_cast<_float*>(&m_Cell[1]), "%.2f") && m_bCellModifyMode)
	{
		m_Cells[m_iCurrentListBoxIndex * 3 + 1].second = m_Cell[1];
		Remake_Cells();
	}
	SetNextItemWidth(150.f);
	if (ImGui::InputFloat3("Cell 2", reinterpret_cast<_float*>(&m_Cell[2]), "%.2f") && m_bCellModifyMode)
	{
		m_Cells[m_iCurrentListBoxIndex * 3 + 2].second = m_Cell[2];
		Remake_Cells();
	}

	return S_OK;
}

HRESULT CWindow_Navigation::Navigation_List()
{
	SetNextItemWidth(150.f);
	if (ImGui::ListBox("Cell Index", &m_iCurrentListBoxIndex, m_CellIndices.data(), (_int)m_CellIndices.size(), 4))
	{
		m_bCellModifyMode = true;
		ZeroMemory(m_Cell, sizeof(_float3) * CCell::POINT_END);
		m_Cell[0] = m_Cells[m_iCurrentListBoxIndex * 3].second;
		m_Cell[1] = m_Cells[m_iCurrentListBoxIndex * 3 + 1].second;
		m_Cell[2] = m_Cells[m_iCurrentListBoxIndex * 3 + 2].second;
	}
	return S_OK;
}

HRESULT CWindow_Navigation::Remake_Cells()
{
	vector<_float3*> Cells;

	for (auto pair = m_Cells.begin(); pair != m_Cells.end();)
	{
		_float3 vPoints[CCell::POINT_END];

		for (_uint i = 0; i < CCell::POINT_END; i++)
		{
			vPoints[i] = (*(pair + i)).second;
		}

		Cells.push_back(vPoints);
		pair += 3;
	}

	m_pTerrain->RemakeCells(Cells);

	return S_OK;
}

HRESULT CWindow_Navigation::NavigationSaveLoad()
{
	return S_OK;
}

HRESULT CWindow_Navigation::NavigationSaveButton()
{
	return S_OK;
}

HRESULT CWindow_Navigation::NavigationWrite_File(const _tchar* pPath)
{
	return S_OK;
}

HRESULT CWindow_Navigation::NavigationLoadButton()
{
	return S_OK;
}

HRESULT CWindow_Navigation::NavigationRead_File(const _tchar* pFileName)
{
	return S_OK;
}

CWindow_Navigation* CWindow_Navigation::Create(ID3D11DeviceContext* pContext, void* pArg)
{
	CWindow_Navigation* pInstance = New CWindow_Navigation(pContext);

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
	Safe_Release(m_pTerrain);
	Safe_Release(m_pContext);

	for (auto& pIndex : m_CellIndices)
	{
		Safe_Delete_Array(pIndex);
	}
}
