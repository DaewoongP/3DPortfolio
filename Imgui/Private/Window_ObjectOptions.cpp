#include "..\Public\Window_ObjectOptions.h"
#include "AnimModel.h"
#include "NonAnimModel.h"
#include "ImWindow_Manager.h"
#include "AnimationNotify.h"
#include "Sound_Manager.h"

CWindow_ObjectOptions::CWindow_ObjectOptions(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

void CWindow_ObjectOptions::Set_CurrentDummy(CDummy::DUMMYTYPE eDummyType, CDummy* pDummy)
{
	m_eCurrentDummyType = eDummyType;

	m_pCurrentDummy = pDummy;

	if (nullptr == pDummy)
	{
		m_pCurrentModel = nullptr;
		return;
	}
	
	m_pCurrentModel = dynamic_cast<CModel*>(pDummy->Find_Component(TEXT("Com_Model")));

	if (dynamic_cast<CAnimModel*>(m_pCurrentDummy))
	{
		m_iNumAnimations = m_pCurrentModel->Get_NumAnimations();

		m_pAnimationNotify->Set_CurrentAnimationObject(static_cast<CAnimModel*>(m_pCurrentDummy), m_pCurrentModel);
	}
}

HRESULT CWindow_ObjectOptions::Initialize(void* pArg)
{
	m_pAnimationNotify = CAnimationNotify::Create();

	ZEROMEM(&m_SphereDesc);
	ZEROMEM(&m_AABBDesc);
	ZEROMEM(&m_OBBDesc);

	return S_OK;
}

void CWindow_ObjectOptions::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	ImGui::Begin("Object Options", nullptr, m_WindowFlag);

	if (ImGui::BeginTabBar("ObjectOptionsTab"))
	{
		if (ImGui::BeginTabItem("Animation"))
		{
			if (nullptr != m_pCurrentModel &&
				CDummy::DUMMY_ANIM == m_eCurrentDummyType)
			{
				AnimationIndex();

				AnimationPause();

				AnimationSpeed(dTimeDelta);
			}

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Collider"))
		{
			if (nullptr != m_pCurrentModel &&
				CDummy::DUMMY_END != m_eCurrentDummyType)
			{
				AddCollider();

				ColliderSaveLoad();
			}

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Sound"))
		{
			if (nullptr != m_pCurrentModel &&
				CDummy::DUMMY_ANIM == m_eCurrentDummyType)
			{

			}

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}

HRESULT CWindow_ObjectOptions::AnimationIndex()
{
	_char szNum[MAX_STR] = "";
	// 인덱스이므로 전체 개수 - 1
	_itoa_s(m_iNumAnimations - 1, szNum, MAX_STR, 10);

	_int iIndex = m_pCurrentDummy->Get_PreToolAnimationIndex();
	CAnimation* pCurrentAnimation = m_pCurrentModel->Get_Animation();

	WCharToChar(pCurrentAnimation->Get_AnimationName(), m_szAnimationName);

	m_iAnimationFrames = pCurrentAnimation->Get_AnimationFrames();
	m_iCurrentAnimationFrame = pCurrentAnimation->Get_CurrentAnimationFrame();

	SetNextItemWidth(100.f);
	if (ImGui::InputInt("Animation Index", &iIndex))
	{
		// 정지버튼 비활성화
		m_bPauseButton = false;
		pCurrentAnimation->Set_Pause(false);
		// 예외처리
		if (m_iNumAnimations - 1 < (_uint)iIndex ||
			0 > iIndex)
			return E_FAIL;
		else
		{
			m_iAnimationIndex = iIndex;
			m_pCurrentModel->Set_AnimIndex(m_iAnimationIndex);
			m_pCurrentDummy->Set_PreToolAnimationIndex(m_iAnimationIndex);
			m_pAnimationNotify->Set_CurrentAnimationObject(static_cast<CAnimModel*>(m_pCurrentDummy), m_pCurrentModel);
		}
	}

	SameLine();
	ImGui::Text("Max : ");

	SameLine();
	ImGui::TextColored(ImVec4(1.f, 0.f, 1.f, 1.f), szNum);
	return S_OK;
}

HRESULT CWindow_ObjectOptions::AnimationPause()
{
	CAnimation* pCurrentAnimation = m_pCurrentModel->Get_Animation();

	if (!m_bPauseButton)
	{
		if (ImGui::Button("Pause"))
		{
			m_bPauseButton = true;
			pCurrentAnimation->Set_Pause(true);
		}
	}
	else
	{
		if (ImGui::Button("Start"))
		{
			m_bPauseButton = false;
			pCurrentAnimation->Set_Pause(false);
		}
	}

	SameLine();
	
	ImGui::Text(m_szAnimationName);

	return S_OK;
}

HRESULT CWindow_ObjectOptions::AnimationSpeed(_double dTimeDelta)
{
	_float progress = (_float)m_iCurrentAnimationFrame / (m_iAnimationFrames - 1);
	ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
	ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
	ImGui::Text("Animation Progress");

	_char buf[MAX_STR] = "";
	sprintf_s(buf, "%d/%d", m_iCurrentAnimationFrame, (m_iAnimationFrames - 1));
	ImGui::ProgressBar(progress, ImVec2(0.f, 0.f), buf);

	// 해당 애니메이션의 특정 프레임에서 처리할 정보들
	if (nullptr != m_pCurrentModel && 
		nullptr != m_pCurrentDummy &&
		CDummy::DUMMY_ANIM == m_eCurrentDummyType &&
		0 < m_iNumAnimations &&
		0 < m_iAnimationFrames)
	{
		MODELWINDOW->Set_Picking(false);

		AnimationNotify(dTimeDelta);
	}
	
	return S_OK;
}

HRESULT CWindow_ObjectOptions::AnimationNotify(_double dTimeDelta)
{
	m_pAnimationNotify->Tick(dTimeDelta);

	return S_OK;
}

HRESULT CWindow_ObjectOptions::AddCollider()
{
	_bool UseCurrentCollider = m_pCurrentDummy->Get_ColliderInMap();

	if (ImGui::Checkbox("Apply Collider", &UseCurrentCollider))
	{
		if (nullptr == m_pCollider)
		{
			m_pCurrentDummy->Set_ColliderInMap(UseCurrentCollider);
			MSG_BOX("Failed Apply Collider");
		}
		else
		{
			switch (m_ColliderType)
			{
			case Engine::CCollider::TYPE_SPHERE:
				m_pCurrentDummy->Set_BoundingDesc(m_ColliderType, &m_SphereDesc);
				break;
			case Engine::CCollider::TYPE_AABB:
				m_pCurrentDummy->Set_BoundingDesc(m_ColliderType, &m_AABBDesc);
				break;
			case Engine::CCollider::TYPE_OBB:
				m_pCurrentDummy->Set_BoundingDesc(m_ColliderType, &m_OBBDesc);
				break;
			}

			m_pCurrentDummy->Set_ColliderInMap(UseCurrentCollider);
			OBJECTWINDOW->Set_UseCollider(UseCurrentCollider);
			m_pCurrentDummy->Set_ColliderCom(m_pCollider);
		}
		
	}

	// 콜라이더 타입 설정.
	if (ImGui::RadioButton("Sphere", (_int*)&m_ColliderType, CCollider::TYPE_SPHERE))
	{
		if (nullptr != m_pCollider)
			Safe_Release(m_pCollider);
		m_pCollider = static_cast<CCollider*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Collider_Sphere")));
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("AABB", (_int*)&m_ColliderType, CCollider::TYPE_AABB))
	{
		if (nullptr != m_pCollider)
			Safe_Release(m_pCollider);
		m_pCollider = static_cast<CCollider*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Collider_AABB")));
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("OBB", (_int*)&m_ColliderType, CCollider::TYPE_OBB))
	{
		if (nullptr != m_pCollider)
			Safe_Release(m_pCollider);
		m_pCollider = static_cast<CCollider*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Collider_OBB")));
	}
	
	// 콜라이더 크기 설정.
	switch (m_ColliderType)
	{
	case Engine::CCollider::TYPE_SPHERE:
		SetUpSphere();
		break;
	case Engine::CCollider::TYPE_AABB:
		SetUpAABB();
		break;
	case Engine::CCollider::TYPE_OBB:
		SetUpOBB();
		break;
	}

	return S_OK;
}

HRESULT CWindow_ObjectOptions::SetUpSphere()
{
	// 콜라이더 위치 오프셋 설정.
	if (ImGui::InputFloat3("Position", reinterpret_cast<_float*>(&m_SphereDesc.vPosition.x)))
	{
		m_pCollider->Set_BoundingDesc(&m_SphereDesc);
	}
	
	if (ImGui::InputFloat("Radius", &m_SphereDesc.fRadius))
	{
		m_pCollider->Set_BoundingDesc(&m_SphereDesc);
	}
	
	return S_OK;
}

HRESULT CWindow_ObjectOptions::SetUpAABB()
{
	if (ImGui::InputFloat3("Position", reinterpret_cast<_float*>(&m_AABBDesc.vPosition)))
	{
		m_pCollider->Set_BoundingDesc(&m_AABBDesc);
	}

	if (ImGui::InputFloat3("Extents", reinterpret_cast<_float*>(&m_AABBDesc.vExtents)))
	{
		m_pCollider->Set_BoundingDesc(&m_AABBDesc);
	}

	return S_OK;
}

HRESULT CWindow_ObjectOptions::SetUpOBB()
{
	if (ImGui::InputFloat3("Position", reinterpret_cast<_float*>(&m_OBBDesc.vPosition)))
	{
		m_pCollider->Set_BoundingDesc(&m_OBBDesc);
	}

	if (ImGui::InputFloat3("Extents", reinterpret_cast<_float*>(&m_OBBDesc.vExtents)))
	{
		m_pCollider->Set_BoundingDesc(&m_OBBDesc);
	}

	if (ImGui::InputFloat3("Rotation", reinterpret_cast<_float*>(&m_OBBDesc.vRotation)))
	{
		m_pCollider->Set_BoundingDesc(&m_OBBDesc);
	}

	return S_OK;
}

HRESULT CWindow_ObjectOptions::ColliderSaveLoad()
{
	ImGui::PushID(0);
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(2 / 7.0f, 0.6f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(2 / 7.0f, 0.7f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(2 / 7.0f, 0.8f, 0.8f));
	if (ImGui::Button("Collider Save"))
	{
		// Pick Disable

		IMFILE->OpenDialog("SaveColliderDialog", "Choose Folder", ".Col", "Collider.Col");
	}
	ImGui::PopStyleColor(3);
	ImGui::PopID();

	ColliderSaveButton();

	ImGui::SameLine();
	ImGui::PushID(0);
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.6f, 1.0f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.7f, 1.0f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.8f, 1.0f, 0.8f));

	if (ImGui::Button("Collider Load"))
	{
		// Pick Disable

		IMFILE->OpenDialog("LoadColliderDialog", "Choose File", ".Col", ".");
	}

	ImGui::PopStyleColor(3);
	ImGui::PopID();

	ColliderLoadButton();

	return S_OK;
}

HRESULT CWindow_ObjectOptions::ColliderSaveButton()
{
	// display
	if (IMFILE->Display("SaveColliderDialog"))
	{
		// action if OK
		if (IMFILE->IsOk())
		{
			string filePath = IMFILE->GetFilePathName();
			_tchar wszPath[MAX_PATH] = TEXT("");
			CharToWChar(filePath.c_str(), wszPath);
			if (FAILED(ColliderWrite_File(wszPath)))
				MSG_BOX("Failed File Write");
		}

		// close
		IMFILE->Close();
	}

	return S_OK;
}

HRESULT CWindow_ObjectOptions::ColliderWrite_File(const _tchar* pPath)
{
	HANDLE hFile = CreateFile(pPath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong	dwByte = 0;

	WriteFile(hFile, &m_ColliderType, sizeof(CCollider::TYPE), &dwByte, nullptr);

	switch (m_ColliderType)
	{
	case Engine::CCollider::TYPE_SPHERE:
		WriteFile(hFile, &m_SphereDesc, sizeof(CBounding_Sphere::BOUNDINGSPHEREDESC), &dwByte, nullptr);
		break;
	case Engine::CCollider::TYPE_AABB:
		WriteFile(hFile, &m_AABBDesc, sizeof(CBounding_AABB::BOUNDINGAABBDESC), &dwByte, nullptr);
		break;
	case Engine::CCollider::TYPE_OBB:
		WriteFile(hFile, &m_OBBDesc, sizeof(CBounding_OBB::BOUNDINGOBBDESC), &dwByte, nullptr);
		break;
	}
	
	CloseHandle(hFile);

	MSG_BOX("File Save Success");

	return S_OK;
}

HRESULT CWindow_ObjectOptions::ColliderLoadButton()
{
	// display
	if (IMFILE->Display("LoadColliderDialog"))
	{
		// action if OK
		if (IMFILE->IsOk())
		{
			map<string, string> strMap = IMFILE->GetSelection();
			string filePathName = IMFILE->GetFilePathName();
			_tchar wszName[MAX_PATH] = TEXT("");
			CharToWChar(filePathName.c_str(), wszName);
			if (FAILED(ColliderRead_File(wszName)))
				MSG_BOX("Failed File Read");
		}

		// close
		IMFILE->Close();
	}

	return S_OK;
}

HRESULT CWindow_ObjectOptions::ColliderRead_File(const _tchar* pFileName)
{
	HANDLE hFile = CreateFile(pFileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong	dwByte = 0;

	if (nullptr != m_pCollider)
		Safe_Release(m_pCollider);

	ReadFile(hFile, &m_ColliderType, sizeof(CCollider::TYPE), &dwByte, nullptr);

	switch (m_ColliderType)
	{
	case Engine::CCollider::TYPE_SPHERE:
		ReadFile(hFile, &m_SphereDesc, sizeof(CBounding_Sphere::BOUNDINGSPHEREDESC), &dwByte, nullptr);
		m_pCollider = static_cast<CCollider*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Collider_Sphere")));
		m_pCollider->Set_BoundingDesc(&m_SphereDesc);
		break;
	case Engine::CCollider::TYPE_AABB:
		ReadFile(hFile, &m_AABBDesc, sizeof(CBounding_AABB::BOUNDINGAABBDESC), &dwByte, nullptr);
		m_pCollider = static_cast<CCollider*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Collider_AABB")));
		m_pCollider->Set_BoundingDesc(&m_AABBDesc);
		break;
	case Engine::CCollider::TYPE_OBB:
		ReadFile(hFile, &m_OBBDesc, sizeof(CBounding_OBB::BOUNDINGOBBDESC), &dwByte, nullptr);
		m_pCollider = static_cast<CCollider*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Collider_OBB")));
		m_pCollider->Set_BoundingDesc(&m_OBBDesc);
		break;
	}

	m_pCurrentDummy->Set_ColliderCom(m_pCollider);
	Safe_AddRef(m_pCollider);

	CloseHandle(hFile);

	MSG_BOX("File Load Success");

	return S_OK;
}

CWindow_ObjectOptions* CWindow_ObjectOptions::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CWindow_ObjectOptions* pInstance = New CWindow_ObjectOptions(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CWindow_ObjectOptions");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWindow_ObjectOptions::Free()
{
	__super::Free();
	
	Safe_Release(m_pCollider);
	Safe_Release(m_pAnimationNotify);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
