#include "..\Public\Window_Tool.h"
#include "AnimModel.h"

CWindow_Tool::CWindow_Tool()
{
	ZEROMEM(&m_TerrainSize);
	ZEROMEM(&m_TerrainOverflowSize);
	ZEROMEM(&m_AxisState);
}

HRESULT CWindow_Tool::Initialize(void* pArg)
{
	m_TerrainSize[0] = 500;
	m_TerrainSize[1] = 500;
	m_TerrainOverflowSize[0] = 50000;
	m_TerrainOverflowSize[1] = 50000;

	m_pTerrain = static_cast<CTerrain*>(m_pGameInstance->Find_GameObject(LEVEL_TOOL, TEXT("Layer_Tool"), TEXT("GameObject_Terrain")));
	if (nullptr == m_pTerrain)
		return E_FAIL;
	Safe_AddRef(m_pTerrain);

	m_pCamera_Free = static_cast<CCamera_Free*>(m_pGameInstance->Find_GameObject(LEVEL_TOOL, TEXT("Layer_Tool"), TEXT("GameObject_Camera_Free")));
	if (nullptr == m_pCamera_Free)
		return E_FAIL;
	Safe_AddRef(m_pCamera_Free);

	m_pAxisUI = static_cast<CAxis*>(m_pGameInstance->Find_GameObject(LEVEL_TOOL, TEXT("Layer_Tool"), TEXT("GameObject_Axis_UI")));
	if (nullptr == m_pAxisUI)
		return E_FAIL;
	Safe_AddRef(m_pAxisUI);

	m_pAxisOrigin = static_cast<CAxis*>(m_pGameInstance->Find_GameObject(LEVEL_TOOL, TEXT("Layer_Tool"), TEXT("GameObject_Axis_Origin")));
	if (nullptr == m_pAxisOrigin)
		return E_FAIL;
	Safe_AddRef(m_pAxisOrigin);

	m_pAxisCenter = static_cast<CAxis*>(m_pGameInstance->Find_GameObject(LEVEL_TOOL, TEXT("Layer_Tool"), TEXT("GameObject_Axis_Center")));
	if (nullptr == m_pAxisCenter)
		return E_FAIL;
	Safe_AddRef(m_pAxisCenter);

	m_AxisState[CAxis::AXIS_ORIGIN] = true;
	m_AxisState[CAxis::AXIS_UI] = true;
	m_AxisState[CAxis::AXIS_CENTER] = false;
	m_pAxisCenter->Set_Rendering(m_AxisState[CAxis::AXIS_CENTER]);

	return S_OK;
}

void CWindow_Tool::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	Begin("Tool", nullptr, m_WindowFlag);

	TerrainSizeXZ();

	TerrainHeightMap();
	
	TerrainTexture();

	WireFrame();
	
	CamSpeedAndAxisDist();
	
	AxisRendering();

    End();
}

void CWindow_Tool::TerrainSizeXZ()
{
	SetNextItemWidth(100.f);
	if (InputInt2("Terrain Size X, Z", m_TerrainSize.data(), ImGuiInputTextFlags_CharsNoBlank))
	{
		if (m_TerrainOverflowSize[0] < m_TerrainSize[0] ||
			0 >= m_TerrainSize[0])
		{
			m_TerrainSize[0] = 5;
			MSG_BOX("Wrong Terrain Size X");
		}

		else if (m_TerrainOverflowSize[1] < m_TerrainSize[1] ||
			0 >= m_TerrainSize[1])
		{
			m_TerrainSize[1] = 5;
			MSG_BOX("Wrong Terrain Size Z");
		}

		else
			m_pTerrain->RemakeTerrain(m_TerrainSize[0], m_TerrainSize[1]);
	}
}

void CWindow_Tool::WireFrame()
{
	if (Checkbox("Wire Frame", &m_bIsWireFrame))
	{
		m_pTerrain->Set_WireFrame(m_bIsWireFrame);
	}
}

void CWindow_Tool::TerrainHeightMap()
{
	if (Button("Open HeightMap"))
		IMFILE->OpenDialog("ChooseHeightMap", "Choose File", ".bmp", ".");

	// display
	if (IMFILE->Display("ChooseHeightMap"))
	{
		// action if OK
		if (IMFILE->IsOk())
		{
			_tchar pPath[MAX_PATH] = TEXT("");
			CharToWChar(IMFILE->GetFilePathName().c_str(), pPath);

			m_pTerrain->RemakeTerrain(pPath);
		}

		// close
		IMFILE->Close();
	}
}

void CWindow_Tool::TerrainTexture()
{
	_char szNum[MAX_STR] = "";
	_itoa_s(m_pTerrain->Get_NumTextures() - 1, szNum, MAX_STR, 10);
	
	_int iIndex = m_iTextureIndex;
	SetNextItemWidth(100.f);
	if (ImGui::InputInt("Texture Index", &iIndex))
	{
		if (m_pTerrain->Get_NumTextures() - 1 < (_uint)iIndex ||
			0 > iIndex)
		{
			return;
		}
		else
		{
			m_iTextureIndex = iIndex;
			m_pTerrain->Set_NumTexture(m_iTextureIndex);
		}
	}
	SameLine();
	ImGui::Text("Max : ");
	SameLine();
	ImGui::TextColored(ImVec4(1.f, 0.f, 1.f, 1.f), szNum);
}

void CWindow_Tool::CamSpeedAndAxisDist()
{
	SetNextItemWidth(190.f);
	if (SliderFloat("FreeCam Speed", &m_fFreeCamSpeed, 5.f, 200.f))
	{
		m_pCamera_Free->Set_Speed(m_fFreeCamSpeed);
	}

	SameLine();
	SetNextItemWidth(100.f);
	if (SliderFloat("Axis Distance", &m_fAxisDistance, 1.f, 10.f))
	{
		m_pAxisCenter->Set_AxisDistance(m_fAxisDistance);
	}
}

void CWindow_Tool::AxisRendering()
{
	if (Checkbox("Origin Axis", &m_AxisState[CAxis::AXIS_ORIGIN]))
	{
		m_pAxisOrigin->Set_Rendering(m_AxisState[CAxis::AXIS_ORIGIN]);
	}
	SameLine();
	if (Checkbox("UI Axis", &m_AxisState[CAxis::AXIS_UI]))
	{
		m_pAxisUI->Set_Rendering(m_AxisState[CAxis::AXIS_UI]);
	}
	SameLine();
	if (Checkbox("Center Axis", &m_AxisState[CAxis::AXIS_CENTER]))
	{
		m_pAxisCenter->Set_Rendering(m_AxisState[CAxis::AXIS_CENTER]);
	}
}

CWindow_Tool* CWindow_Tool::Create(void* pArg)
{
	CWindow_Tool* pInstance = New CWindow_Tool();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CWindow_Tool");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWindow_Tool::Free()
{
	__super::Free();
	Safe_Release(m_pTerrain);
	Safe_Release(m_pCamera_Free);
	Safe_Release(m_pAxisUI);
	Safe_Release(m_pAxisCenter);
	Safe_Release(m_pAxisOrigin);
}

