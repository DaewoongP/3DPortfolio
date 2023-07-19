#include "..\Public\Window_Light.h"
#include "GameInstance.h"
#include "Camera_Free.h"

CWindow_Light::CWindow_Light()
{
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
	m_LightDesc.fRange = 1.f;
	m_LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	m_LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	m_LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	// 처음에 그냥 방향 빛을 띄우기 위함
	CLight::LIGHTDESC		LightDesc;
	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = CLight::TYPE_DIRECTIONAL;
	LightDesc.vDir = _float4(1.f, -1.f, 1.f, 0.f);

	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Lights(LightDesc)))
		return E_FAIL;

	return S_OK;
}

void CWindow_Light::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	// 콜라이더 렌더링.
	for (auto& pair : m_Lights)
	{
		if (nullptr != pair.first)
			m_pRenderer->Add_DebugGroup(pair.first);
	}

	Begin("Light", nullptr, m_WindowFlag);

	SetUp_LightDesc();

	End();
}

HRESULT CWindow_Light::SetUp_LightDesc()
{
	ImGui::InputFloat3("Light Position", reinterpret_cast<_float*>(&m_LightDesc.vPos));
	ImGui::InputFloat("Light Range", &m_LightDesc.fRange, 1.f);
	ImGui::InputFloat3("Light Diffuse", reinterpret_cast<_float*>(&m_LightDesc.vDiffuse));
	ImGui::InputFloat3("Light Ambient", reinterpret_cast<_float*>(&m_LightDesc.vAmbient));
	ImGui::InputFloat3("Light Specular", reinterpret_cast<_float*>(&m_LightDesc.vSpecular));

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

		memcpy(&SphereDesc.vPosition, &m_LightDesc.vPos, sizeof(_float3));
		SphereDesc.fRadius = m_LightDesc.fRange;

		_char* szIndex = New _char[6];
		_itoa_s(m_iCurrentLightIndex, szIndex, sizeof(6), 10);
		++m_iCurrentLightIndex;
		m_LightIndices.push_back(szIndex);

		m_Lights.push_back({ static_cast<CCollider*>(m_pOriginCollider->Clone(&SphereDesc)), m_LightDesc });


		if (true == m_isLight)
		{
			m_pGameInstance->Clear_Lights();

			for (auto& pair : m_Lights)
			{
				if (FAILED(m_pGameInstance->Add_Lights(pair.second)))
					return E_FAIL;
			}
		}
	}
	SameLine();
	if (ImGui::Checkbox("Show Lights", &m_isLight))
	{
		m_pGameInstance->Clear_Lights();

		// Directional Light
		if (false == m_isLight)
		{
			CLight::LIGHTDESC		LightDesc;
			ZeroMemory(&LightDesc, sizeof LightDesc);

			LightDesc.eType = CLight::TYPE_DIRECTIONAL;
			LightDesc.vDir = _float4(1.f, -1.f, 1.f, 0.f);

			LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
			LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
			LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

			if (FAILED(m_pGameInstance->Add_Lights(LightDesc)))
				return E_FAIL;
		}
		else
		{
			for (auto& pair : m_Lights)
			{
				if (FAILED(m_pGameInstance->Add_Lights(pair.second)))
					return E_FAIL;
			}
		}
	}

	if (ImGui::ListBox("Light Index", &m_iCurrentListBoxIndex, m_LightIndices.data(), (_int)m_LightIndices.size(), 4))
	{
		CCamera_Free* pCam = dynamic_cast<CCamera_Free*>(m_pGameInstance->Find_GameObject(LEVEL_TOOL, TEXT("Layer_Tool"), TEXT("GameObject_Camera_Free")));

		if (nullptr == pCam)
			return E_FAIL;

		_float4 vCamPos, vPos;
		vPos = m_Lights[m_iCurrentListBoxIndex].second.vPos;
		vPos.w = 1.f;
		vCamPos = vPos;
		vCamPos.y += 10.f;
		vCamPos.z -= 10.f;
		pCam->Set_CameraView(vCamPos, vPos, _float4(0.f, 1.f, 0.f, 0.f));
	}

	return S_OK;
}

HRESULT CWindow_Light::Delete_LightDesc()
{
	return E_NOTIMPL;
}

CWindow_Light* CWindow_Light::Create(void* pArg)
{
	CWindow_Light* pInstance = New CWindow_Light();

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
	Safe_Release(m_pRenderer);
}
