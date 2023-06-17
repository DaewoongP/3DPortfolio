#include "..\Public\Window_Object.h"
#include "Layer.h"
#include "Dummy.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "ImWindow_Manager.h"
#include "AnimModel.h"
#include "Camera_Free.h"
#include "AnimModel.h"

CWindow_Object::CWindow_Object()
{
}

void CWindow_Object::Set_Object(CGameObject* pObject)
{
	if (nullptr == pObject)
		return;
	m_Objects.push_back(pObject);
	Safe_AddRef(pObject);
	_char* szName = New _char[lstrlen(pObject->Get_Tag()) + 1];
	WCharToChar(pObject->Get_Tag(), szName);
	m_ObjectNames.push_back(szName);
}

HRESULT CWindow_Object::Initialize(void* pArg)
{
	m_vWindowPos = ImVec2(-870, g_iWinSizeY);
	m_vWindowSize = ImVec2(400, 300);

	return S_OK;
}

void CWindow_Object::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
	Begin("Object", nullptr, m_WindowFlag);

	CurrentObjectListBox();

	DeleteObject();

	AnimationIndex();

	End();
}

HRESULT CWindow_Object::CurrentObjectListBox()
{
	ImGui::SetNextItemWidth(300.f);
	if (ImGui::ListBox("Objects", &m_iCurrentListIndex, m_ObjectNames.data(), (_int)m_ObjectNames.size(), 5))
	{
		m_iAnimationIndex = 0;

		CDummy* pDummy = dynamic_cast<CDummy*>(m_Objects[m_iCurrentListIndex]);
		CCamera_Free* pCam = dynamic_cast<CCamera_Free*>(m_pGameInstance->Find_GameObject(LEVEL_TOOL, TEXT("Layer_Tool"), TEXT("GameObject_Camera_Free")));
		
		if (nullptr == pDummy ||
			nullptr == pCam)
			return E_FAIL;

		m_pGameInstance->Set_LastGameObject(m_Objects[m_iCurrentListIndex]);
		MODELWINDOW->Set_InputScale(pDummy->Get_PreToolScale());
		MODELWINDOW->Set_InputRotation(pDummy->Get_PreToolRotation());
		MODELWINDOW->Set_InputTransform(pDummy->Get_PreToolTransform());

		_float4 vTransform = pDummy->Get_PreToolTransform();
		vTransform.y += 10.f;
		vTransform.z += 10.f;
		pCam->Set_CameraView(vTransform, pDummy->Get_PreToolTransform(), _float4(0.f, 1.f, 0.f, 0.f));
	}
	return S_OK;
}

HRESULT CWindow_Object::DeleteObject()
{
	if (ImGui::Button("Delete Select Object"))
	{
		_int iIndex = { 0 };
		for (auto iter = m_Objects.begin(); iter != m_Objects.end();)
		{
			if (m_iCurrentListIndex == iIndex)
			{
				m_pGameInstance->Delete_GameObject(LEVEL_TOOL, TEXT("Layer_Tool"), (*iter)->Get_Tag());
				Safe_Release(*iter);
				iter = m_Objects.erase(iter);
				break;
			}
			else
			{
				++iter;
				++iIndex;
			}
		}
		iIndex = 0;
		for (auto iter = m_ObjectNames.begin(); iter != m_ObjectNames.end();)
		{
			if (m_iCurrentListIndex == iIndex)
			{
				Safe_Delete_Array(*iter);
				iter = m_ObjectNames.erase(iter);
				break;
			}
			else
			{
				++iter;
				++iIndex;
			}
		}
		if (0 < m_iCurrentListIndex)
			--m_iCurrentListIndex;
	}

	if (!m_bClearButton)
	{
		if (ImGui::Button("Clear ALL Object"))
		{
			m_bClearButton = true;
		}
	}	
	else
	{
		SameLine();
		if (ImGui::Button("Clear ALL"))
		{
			for (auto& iter : m_Objects)
			{
				m_pGameInstance->Delete_GameObject(LEVEL_TOOL, TEXT("Layer_Tool"), iter->Get_Tag());
				Safe_Release(iter);
			}
			m_Objects.clear();

			for (auto& iter : m_ObjectNames)
			{
				Safe_Delete_Array(iter);
			}
			m_ObjectNames.clear();

			m_bClearButton = false;
		}
		m_iCurrentListIndex = 0;

		SameLine();
		if (ImGui::Button("Nope"))
		{
			m_bClearButton = false;
		}
	}

	return S_OK;
}

HRESULT CWindow_Object::AnimationIndex()
{
	CAnimModel* pAnimModel = { nullptr };
	if (0 < m_Objects.size() &&
		(pAnimModel = dynamic_cast<CAnimModel*>(m_Objects[m_iCurrentListIndex])))
	{
		_char szNum[MAX_STR] = "";
		_itoa_s(pAnimModel->Get_NumAnimations() - 1, szNum, MAX_STR, 10);

		_int iIndex = pAnimModel->Get_PreToolAnimationIndex();

		SetNextItemWidth(100.f);
		if (ImGui::InputInt("Animation Index", &iIndex))
		{
			if (pAnimModel->Get_NumAnimations() - 1 < (_uint)iIndex ||
				0 > iIndex)
			{
				return E_FAIL;
			}
			else
			{
				m_iAnimationIndex = iIndex;
				pAnimModel->Set_AnimIndex(m_iAnimationIndex);
				pAnimModel->Set_PreToolAnimationIndex(m_iAnimationIndex);
			}
		}
		SameLine();
		ImGui::Text("Max : ");
		SameLine();
		ImGui::TextColored(ImVec4(1.f, 0.f, 1.f, 1.f), szNum);

		AnimationSpeed(pAnimModel);
	}

	return S_OK;
}

HRESULT CWindow_Object::AnimationSpeed(CAnimModel* pAnimModel)
{
	m_dAnimationSpeed = pAnimModel->Get_PreToolAnimationSpeed();
	if (ImGui::InputDouble("Animation Speed", &m_dAnimationSpeed))
	{
		pAnimModel->Set_AnimationSpeed(m_dAnimationSpeed);
		pAnimModel->Set_PreToolAnimationSpeed(m_dAnimationSpeed);
	}
	
	return S_OK;
}

CWindow_Object* CWindow_Object::Create(void* pArg)
{
	CWindow_Object* pInstance = New CWindow_Object();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CWindow_Object");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWindow_Object::Free()
{
	__super::Free();

	for (auto& iter : m_Objects)
	{
		Safe_Release(iter);
	}
	m_Objects.clear();

	for (auto& iter : m_ObjectNames)
	{
		Safe_Delete_Array(iter);
	}
	m_ObjectNames.clear();
}
