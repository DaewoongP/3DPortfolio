#include "..\Public\Window_Light.h"
#include "GameInstance.h"
#include "Camera_Free.h"
#include "Terrain.h"

CWindow_Light::CWindow_Light(ID3D11DeviceContext* pContext)
	: m_pContext(pContext)
{
	Safe_AddRef(m_pContext);
}

HRESULT CWindow_Light::Initialize(void* pArg)
{
	// 클론하기위한 콜라이더를 미리 받아놓고 나중에 클론하면서 툴에 띄울 예정.
	m_pOriginCollider = dynamic_cast<CCollider*>(m_pGameInstance->Find_PrototypesBySubTag(LEVEL_TOOL, TEXT("Collider_Sphere")).begin()->second);
	Safe_AddRef(m_pOriginCollider);
	m_pOriginCollider->Set_Color(DirectX::Colors::White);

	CComponent* pRenderer = m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Renderer"));
	m_pRenderer = static_cast<CRenderer*>(pRenderer);

	ZEROMEM(&m_LightDesc);
	m_LightDesc.eType = CLight::TYPE_POINT; // 고정
	m_LightDesc.vPos = _float4(0.f, 0.f, 0.f, 1.f);
	m_LightDesc.vDir = _float4(0.f, -1.f, 0.f, 0.f);
	m_LightDesc.fSpotPower = 10.f;
	m_LightDesc.fRange = 1.f;
	m_LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	m_LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	m_LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	Add_DirectionalLight();

	m_pTerrain = static_cast<CTerrain*>(m_pGameInstance->Find_GameObject(LEVEL_TOOL, TEXT("Layer_Tool"), TEXT("GameObject_Terrain")));
	if (nullptr == m_pTerrain)
		return E_FAIL;

	Safe_AddRef(m_pTerrain);

	return S_OK;
}

void CWindow_Light::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	// 콜라이더 렌더링.
	for (auto& pair : m_Lights)
	{
		if (nullptr != pair.first)
		{
			pair.first->Tick(XMMatrixTranslation(pair.second.vPos.x, pair.second.vPos.y, pair.second.vPos.z));

			m_pRenderer->Add_DebugGroup(pair.first);
		}
	}

	Begin("Light", nullptr, m_WindowFlag);

	Find_Position();

	SetUp_LightDesc();

	Delete_LightDesc();

	Pick_Light(dTimeDelta);

	LightSaveLoad();

	End();
}

HRESULT CWindow_Light::Render()
{
	for (_uint i = 0; i < m_Lights.size(); ++i)
	{
		if (i >= m_LightIndices.size())
			break;

		_float3 vLightCenterPos = m_Lights[i].first->Get_BoundingCenterPosition();
		_vector vCenter = XMVectorSet(vLightCenterPos.x, vLightCenterPos.y, vLightCenterPos.z, 1.f);

		_matrix ViewMatrix = m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);
		_matrix ProjMatrix = m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);

		_vector vViewPos = XMVector3TransformCoord(vCenter, ViewMatrix);
		_vector vProjPos = XMVector3TransformCoord(vViewPos, ProjMatrix);

		_uint iVP = 1;
		D3D11_VIEWPORT ViewPort;
		ZEROMEM(&ViewPort);
		m_pContext->RSGetViewports(&iVP, &ViewPort);

		_float3 vRenderCenterPos;
		_float2 vRender;
		XMStoreFloat3(&vRenderCenterPos, vProjPos);
		// 투영 좌표가 아닌 친구들을 렌더링 하지 않게 처리.
		if (-1.f > vRenderCenterPos.x ||
			1.f < vRenderCenterPos.x ||
			-1.f > vRenderCenterPos.y ||
			1.f < vRenderCenterPos.y ||
			0.f > vRenderCenterPos.z ||
			1.f < vRenderCenterPos.z)
			continue;

		_float fDepth = (1.f - vRenderCenterPos.z) * m_Lights[i].second.fRange * 20.f;

		vRender.x = (vRenderCenterPos.x + 1.f) * 0.5f * ViewPort.Width - fDepth * 0.5f;
		vRender.y = (1.f - vRenderCenterPos.y) * 0.5f * ViewPort.Height - fDepth * 0.5f;

		_tchar szLightIndex[MAX_STR] = TEXT("");
		CharToWChar(m_LightIndices[i], szLightIndex);

		if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_135"), szLightIndex, vRender, XMLoadFloat4(&m_Lights[i].second.vDiffuse), 0.f, _float2(0.f, 0.f), fDepth)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CWindow_Light::Find_Position()
{
	ImGui::Checkbox("Picking To Find Position", &m_isPickMode);
	
	if (true == m_isPickMode &&
		m_pGameInstance->IsMouseInClient(m_pContext, g_hWnd) &&
		m_pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_DOWN))
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

		m_LightDesc.vPos = vPickPos;
	}

	return S_OK;
}

HRESULT CWindow_Light::SetUp_LightDesc()
{
	ImGui::RadioButton("Point Light", reinterpret_cast<_int*>(&m_LightDesc.eType), _int(CLight::TYPE_POINT));
	ImGui::SameLine();
	ImGui::RadioButton("Spot Light", reinterpret_cast<_int*>(&m_LightDesc.eType), _int(CLight::TYPE_SPOTLIGHT));

	
	if (ImGui::InputFloat3("Light Position", reinterpret_cast<_float*>(&m_LightDesc.vPos)) ||
		ImGui::InputFloat("Light Range", &m_LightDesc.fRange, 1.f) ||
		ImGui::InputFloat3("Light Diffuse", reinterpret_cast<_float*>(&m_LightDesc.vDiffuse)) ||
		ImGui::InputFloat3("Light Ambient", reinterpret_cast<_float*>(&m_LightDesc.vAmbient)) ||
		ImGui::InputFloat3("Light Specular", reinterpret_cast<_float*>(&m_LightDesc.vSpecular)) ||
		CLight::TYPE_SPOTLIGHT != m_LightDesc.eType ||
		ImGui::InputFloat3("Light Direction", reinterpret_cast<_float*>(&m_LightDesc.vDir)) ||
		ImGui::InputFloat("Light Power", &m_LightDesc.fSpotPower, 1.f))
	{
		if (0 > m_iCurrentListBoxIndex &&
			0 < m_Lights.size() &&
			m_iCurrentListBoxIndex < m_Lights.size() &&
			true == m_isModifyMode)
		{
			m_Lights[m_iCurrentListBoxIndex].second = m_LightDesc;

			CBounding_Sphere::BOUNDINGSPHEREDESC SphereDesc;
			ZEROMEM(&SphereDesc);
			SphereDesc.fRadius = m_LightDesc.fRange;
			m_Lights[m_iCurrentListBoxIndex].first->Set_BoundingDesc(&SphereDesc);
			Remake_Lights();
		}
	}

	if (ImGui::Button("Same As Diffuse"))
	{
		m_LightDesc.vAmbient = m_LightDesc.vDiffuse;
		m_LightDesc.vSpecular = m_LightDesc.vDiffuse;
	}

	if (ImGui::Button("Apply"))
	{
		if (0.f == m_LightDesc.fRange)
		{
			MSG_BOX("Light Range is Zero");
			m_LightDesc.fRange = 1.f;
			return S_OK;
		}
		
		CBounding_Sphere::BOUNDINGSPHEREDESC SphereDesc;
		ZEROMEM(&SphereDesc);

		SphereDesc.fRadius = m_LightDesc.fRange;

		_char* szIndex = New _char[6];
		_itoa_s(m_iCurrentLightIndex, szIndex, sizeof(6), 10);
		++m_iCurrentLightIndex;
		m_LightIndices.push_back(szIndex);

		m_pOriginCollider->Set_Color(XMLoadFloat4(&m_LightDesc.vDiffuse));
		m_Lights.push_back({ static_cast<CCollider*>(m_pOriginCollider->Clone(&SphereDesc)), m_LightDesc });
		
		if (true == m_isLight)
		{
			Remake_Lights();
		}
	}
	SameLine();
	if (ImGui::Checkbox("Show Lights", &m_isLight))
	{
		// Directional Light
		if (false == m_isLight)
		{
			m_pGameInstance->Clear_Lights();

			CLight::LIGHTDESC		LightDesc;
			ZeroMemory(&LightDesc, sizeof LightDesc);

			LightDesc.eType = CLight::TYPE_DIRECTIONAL;
			LightDesc.vDir = _float4(1.f, -1.f, 1.f, 0.f);

			LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
			LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
			LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
			m_pGameInstance->Add_Lights(LightDesc);
		}
		else
		{
			Remake_Lights();
		}
	}

	if (ImGui::ListBox("Light Index", &m_iCurrentListBoxIndex, m_LightIndices.data(), (_int)m_LightIndices.size(), 4))
	{
		if (0 == m_Lights.size() &&
			m_iCurrentListBoxIndex >= m_Lights.size() &&
			0 > m_iCurrentListBoxIndex)
			return E_FAIL;

		m_isModifyMode = false;

		// 값 화면에 띄우기 위한 복사.
		memcpy(&m_LightDesc, &m_Lights[m_iCurrentListBoxIndex].second, sizeof(CLight::LIGHTDESC));

		CCamera_Free* pCam = dynamic_cast<CCamera_Free*>(m_pGameInstance->Find_GameObject(LEVEL_TOOL, TEXT("Layer_Tool"), TEXT("GameObject_Camera_Free")));

		if (nullptr == pCam)
			return E_FAIL;

		_float4 vCamPos, vPos;
		vPos = m_Lights[m_iCurrentListBoxIndex].second.vPos;
		vPos.w = 1.f;
		vCamPos = vPos;
		vCamPos.y += m_Lights[m_iCurrentListBoxIndex].second.fRange;
		vCamPos.z -= m_Lights[m_iCurrentListBoxIndex].second.fRange;
		pCam->Set_CameraView(vCamPos, vPos, _float4(0.f, 1.f, 0.f, 0.f));
	}

	return S_OK;
}

HRESULT CWindow_Light::Delete_LightDesc()
{
	if (ImGui::Button("Delete Light"))
	{
		_int iIndex = { 0 };
		for (auto pair = m_Lights.begin(); pair != m_Lights.end();)
		{
			if (m_iCurrentListBoxIndex == iIndex)
			{
				Safe_Release(pair->first);
				pair = m_Lights.erase(pair);
				break;
			}
			else
			{
				++pair;
				++iIndex;
			}
		}
		iIndex = 0;

		for (auto iter = m_LightIndices.begin(); iter != m_LightIndices.end();)
		{
			if (m_iCurrentListBoxIndex == iIndex)
			{
				Safe_Delete_Array(*iter);
				iter = m_LightIndices.erase(iter);
				break;
			}
			else
			{
				++iter;
				++iIndex;
			}
		}

		Remake_Lights();
	}

	if (!m_bClearButton)
	{
		if (ImGui::Button("Clear Light"))
		{
			m_bClearButton = true;
		}
	}
	else
	{
		if (ImGui::Button("Clear ALL"))
		{
			for (auto& pair : m_Lights)
				Safe_Release(pair.first);
			m_Lights.clear();

			for (auto& iter : m_LightIndices)
				Safe_Delete_Array(iter);
			m_LightIndices.clear();

			m_isLight = false;
			m_pGameInstance->Clear_Lights();
			Add_DirectionalLight();
		}

		m_iCurrentListBoxIndex = 0;

		SameLine();
		if (ImGui::Button("Nope"))
		{
			m_bClearButton = false;
		}
	}

	return S_OK;
}

HRESULT CWindow_Light::Pick_Light(_double dTimeDelta)
{
	if (ImGui::Checkbox("Modify Light", &m_isModifyMode))
	{
		if (m_iCurrentListBoxIndex >= m_Lights.size())
			return E_FAIL;
		memcpy(&m_LightDesc, &m_Lights[m_iCurrentListBoxIndex].second, sizeof(CLight::LIGHTDESC));
	}

	if (true == m_isModifyMode)
	{
		if (m_pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_DOWN) &&
			m_pGameInstance->IsMouseInClient(m_pContext, g_hWnd))
		{
			m_iPickingIndex = isPickLight();
		}
		if (m_pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_PRESSING) &&
			m_pGameInstance->IsMouseInClient(m_pContext, g_hWnd) &&
			-1 != m_iPickingIndex)
		{
			_long dwMouseMove = { 0 };
			if (m_pGameInstance->Get_DIKeyState(DIK_LSHIFT, CInput_Device::KEY_PRESSING))
			{
				// shift 누르면 X축 이동
				dwMouseMove = m_pGameInstance->Get_DIMouseMove(CInput_Device::DIMM_X);
				if (0 != dwMouseMove)
				{
					m_Lights[m_iPickingIndex].second.vPos.x += _float(dwMouseMove * dTimeDelta * m_Lights[m_iPickingIndex].second.fRange * 0.5f);

					Remake_Lights();
				}
			}
			else
			{
				dwMouseMove = m_pGameInstance->Get_DIMouseMove(CInput_Device::DIMM_Y);
				if (0 != dwMouseMove)
				{
					m_Lights[m_iPickingIndex].second.vPos.z -= _float(dwMouseMove * dTimeDelta * m_Lights[m_iPickingIndex].second.fRange * 0.5f);

					Remake_Lights();
				}
			}
		}
		if (m_pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_UP))
		{
			if (0 > m_iPickingIndex)
				return S_OK;
			memcpy(&m_LightDesc.vPos, &m_Lights[m_iPickingIndex].second.vPos, sizeof(_float3));
			m_iPickingIndex = -1;
		}
	}

	return S_OK;
}

HRESULT CWindow_Light::LightSaveLoad()
{
	ImGui::PushID(0);
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(2 / 7.0f, 0.6f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(2 / 7.0f, 0.7f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(2 / 7.0f, 0.8f, 0.8f));
	if (ImGui::Button("Light Save"))
	{
		// Pick Disable
		m_isPickMode = false;
		IMFILE->OpenDialog("SaveLightDialog", "Choose Folder", ".Light", "Light.Light");
	}
	ImGui::PopStyleColor(3);
	ImGui::PopID();

	LightSaveButton();

	ImGui::SameLine();
	ImGui::PushID(0);
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.6f, 1.0f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.7f, 1.0f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.8f, 1.0f, 0.8f));

	if (ImGui::Button("Light Load"))
	{
		// Pick Disable
		m_isPickMode = false;
		IMFILE->OpenDialog("LoadLightDialog", "Choose File", ".Light", ".");
	}

	ImGui::PopStyleColor(3);
	ImGui::PopID();

	LightLoadButton();

	return S_OK;
}

HRESULT CWindow_Light::LightSaveButton()
{
	// display
	if (IMFILE->Display("SaveLightDialog"))
	{
		// action if OK
		if (IMFILE->IsOk())
		{
			string filePath = IMFILE->GetFilePathName();
			_tchar wszPath[MAX_PATH] = TEXT("");
			CharToWChar(filePath.c_str(), wszPath);
			if (FAILED(LightWrite_File(wszPath)))
				MSG_BOX("Failed File Write");
		}

		// close
		IMFILE->Close();
	}

	return S_OK;
}

HRESULT CWindow_Light::LightWrite_File(const _tchar* pPath)
{
	HANDLE hFile = CreateFile(pPath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong	dwByte = 0;

	// 혹시 모를 빛의 개수 저장.
	_uint iSize = (_uint)m_Lights.size();
	WriteFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

	for (auto& pair : m_Lights)
	{
		WriteFile(hFile, &pair.second, sizeof(CLight::LIGHTDESC), &dwByte, nullptr);
	}

	CloseHandle(hFile);

	MSG_BOX("File Save Success");

	return S_OK;
}

HRESULT CWindow_Light::LightLoadButton()
{
	// display
	if (IMFILE->Display("LoadLightDialog"))
	{
		// action if OK
		if (IMFILE->IsOk())
		{
			map<string, string> strMap = IMFILE->GetSelection();
			string filePathName = IMFILE->GetFilePathName();
			_tchar wszName[MAX_PATH] = TEXT("");
			CharToWChar(filePathName.c_str(), wszName);
			if (FAILED(LightRead_File(wszName)))
				MSG_BOX("Failed File Read");
		}

		// close
		IMFILE->Close();
	}

	return S_OK;
}

HRESULT CWindow_Light::LightRead_File(const _tchar* pFileName)
{
	HANDLE hFile = CreateFile(pFileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong	dwByte = 0;

	_uint iSize = { 0 };
	ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

	CLight::LIGHTDESC LightDesc;
	CBounding_Sphere::BOUNDINGSPHEREDESC SphereDesc;

	for (_uint i = 0;  i < iSize; ++i)
	{
		ZEROMEM(&LightDesc);
		ReadFile(hFile, &LightDesc, sizeof(CLight::LIGHTDESC), &dwByte, nullptr);

		ZEROMEM(&SphereDesc);
		SphereDesc.fRadius = LightDesc.fRange;

		_char* szIndex = New _char[6];
		_itoa_s(m_iCurrentLightIndex, szIndex, sizeof(6), 10);
		++m_iCurrentLightIndex;
		m_LightIndices.push_back(szIndex);

		m_pOriginCollider->Set_Color(XMLoadFloat4(&LightDesc.vDiffuse));
		m_Lights.push_back({ static_cast<CCollider*>(m_pOriginCollider->Clone(&SphereDesc)), LightDesc });
	}

	CloseHandle(hFile);

	MSG_BOX("File Load Success");

	return S_OK;
}

void CWindow_Light::Add_DirectionalLight()
{
	CLight::LIGHTDESC		LightDesc;
	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = CLight::TYPE_DIRECTIONAL;
	LightDesc.vDir = _float4(1.f, -1.f, 1.f, 0.f);

	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	m_pGameInstance->Add_Lights(LightDesc);
}

void CWindow_Light::Remake_Lights()
{
	m_pGameInstance->Clear_Lights();

	// 살짝 밝게
	CLight::LIGHTDESC		LightDesc;
	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = CLight::TYPE_DIRECTIONAL;
	LightDesc.vDir = _float4(1.f, -1.f, 1.f, 0.f);

	LightDesc.vDiffuse = _float4(0.2f, 0.2f, 0.2f, 1.f);
	LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 1.f);
	LightDesc.vSpecular = _float4(0.2f, 0.2f, 0.2f, 1.f);

	m_pGameInstance->Add_Lights(LightDesc);

	for (auto& pair : m_Lights)
	{
		m_pGameInstance->Add_Lights(pair.second);
	}
}

_int CWindow_Light::isPickLight()
{
	_float4 vRayPos, vRayDir;

	if (FAILED(m_pGameInstance->Get_MouseRay(m_pContext, g_hWnd, XMMatrixIdentity(), &vRayPos, &vRayDir)))
		return false;

	_float fIntersectsDistance = 999.f;
	_int iIndex = { -1 };
	_int iIntersectsIndex = { -1 };
	for (auto& pair : m_Lights)
	{
		++iIndex;
		_float fDist = 1000.f;

		if (true == pair.first->RayIntersects(XMLoadFloat4(&vRayPos), XMLoadFloat4(&vRayDir), fDist))
		{
			if (0.f < fDist && fIntersectsDistance > fDist)
			{
				fIntersectsDistance = fDist;
				iIntersectsIndex = iIndex;
			}
		}
	}

	return iIntersectsIndex;
}

CWindow_Light* CWindow_Light::Create(ID3D11DeviceContext* pContext, void* pArg)
{
	CWindow_Light* pInstance = New CWindow_Light(pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CWindow_Light");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWindow_Light::Free()
{
	__super::Free();

	for (auto& pair : m_Lights)
		Safe_Release(pair.first);
	m_Lights.clear();

	for (auto& iter : m_LightIndices)
		Safe_Delete_Array(iter);
	m_LightIndices.clear();

	Safe_Release(m_pOriginCollider);
	Safe_Release(m_pTerrain);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pContext);
}
