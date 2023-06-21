#include "..\Public\Window_Object.h"
#include "Layer.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "ImWindow_Manager.h"
#include "AnimModel.h"
#include "Camera_Free.h"
#include "AnimModel.h"
#include "NonAnimModel.h"

CWindow_Object::CWindow_Object()
{
}

void CWindow_Object::Set_Object(CDummy::DUMMYTYPE eType, CGameObject* pObject)
{
	if (nullptr == pObject)
		return;

	m_Objects[eType].push_back(pObject);
	Safe_AddRef(pObject);

	_char* szName = New _char[lstrlen(pObject->Get_Tag()) + 1];
	WCharToChar(pObject->Get_Tag(), szName);

	m_ObjectNames[eType].push_back(szName);
}

HRESULT CWindow_Object::Initialize(void* pArg)
{

	return S_OK;
}

void CWindow_Object::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	Begin("Object", nullptr, m_WindowFlag);

	Select_Objects();

	CurrentObjectListBox();

	SearchObject();

	DeleteObject();

	ObjectOption();

	End();
}

HRESULT CWindow_Object::Select_Objects()
{
	// 오브젝트 리스트 활성화 버튼
	RadioButton("Non Anim", (_int*)(&m_eCurRadio), CDummy::DUMMY_NONANIM);

	SameLine();

	RadioButton("Anim", (_int*)(&m_eCurRadio), CDummy::DUMMY_ANIM);

	return S_OK;
}

HRESULT CWindow_Object::CurrentObjectListBox()
{
	ImGui::SetNextItemWidth(300.f);
	if (ImGui::ListBox("Objects", &m_iCurrentListIndex, m_ObjectNames[m_eCurRadio].data(), (_int)m_ObjectNames[m_eCurRadio].size(), 10))
	{
		// 리스트박스에서 오브젝트를 클릭하면 해당 오브젝트가 있는 곳으로 카메라가 이동
		// 해당 오브젝트의 상태값 (SRT) 도 보여줌.
		CDummy* pDummy = dynamic_cast<CDummy*>(m_Objects[m_eCurRadio][m_iCurrentListIndex]);
		CCamera_Free* pCam = dynamic_cast<CCamera_Free*>(m_pGameInstance->Find_GameObject(LEVEL_TOOL, TEXT("Layer_Tool"), TEXT("GameObject_Camera_Free")));
		
		if (nullptr == pDummy ||
			nullptr == pCam)
			return E_FAIL;

		m_pGameInstance->Set_LastGameObject(m_Objects[m_eCurRadio][m_iCurrentListIndex]);
		MODELWINDOW->Set_InputScale(pDummy->Get_PreToolScale());
		MODELWINDOW->Set_InputRotation(pDummy->Get_PreToolRotation());
		MODELWINDOW->Set_InputTransform(pDummy->Get_PreToolTransform());

		_float4 vTransform = pDummy->Get_PreToolTransform();
		vTransform.y += 6.f;
		vTransform.z -= 5.f;
		pCam->Set_CameraView(vTransform, pDummy->Get_PreToolTransform(), _float4(0.f, 1.f, 0.f, 0.f));
	}

	return S_OK;
}

HRESULT CWindow_Object::SearchObject()
{
	_char test[256] = "";
	ImGui::InputText("Search Object", test, 0);
	// 여기서 파일이름 서칭해서 처리
	return S_OK;
}

HRESULT CWindow_Object::DeleteObject()
{
	if (ImGui::Button("Delete Select Object"))
	{
		_int iIndex = { 0 };
		for (auto iter = m_Objects[m_eCurRadio].begin(); iter != m_Objects[m_eCurRadio].end();)
		{
			if (m_iCurrentListIndex == iIndex)
			{
				m_pGameInstance->Delete_GameObject(LEVEL_TOOL, TEXT("Layer_Tool"), (*iter)->Get_Tag());
				Safe_Release(*iter);
				iter = m_Objects[m_eCurRadio].erase(iter);
				break;
			}
			else
			{
				++iter;
				++iIndex;
			}
		}
		iIndex = 0;
		for (auto iter = m_ObjectNames[m_eCurRadio].begin(); iter != m_ObjectNames[m_eCurRadio].end();)
		{
			if (m_iCurrentListIndex == iIndex)
			{
				Safe_Delete_Array(*iter);
				iter = m_ObjectNames[m_eCurRadio].erase(iter);
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
		if (ImGui::Button("Clear ALL"))
		{
			for (auto& iter : m_Objects[m_eCurRadio])
			{
				m_pGameInstance->Delete_GameObject(LEVEL_TOOL, TEXT("Layer_Tool"), iter->Get_Tag());
				Safe_Release(iter);
			}
			m_Objects[m_eCurRadio].clear();

			for (auto& iter : m_ObjectNames[m_eCurRadio])
			{
				Safe_Delete_Array(iter);
			}
			m_ObjectNames[m_eCurRadio].clear();

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

HRESULT CWindow_Object::ObjectOption()
{
	// 현재 선택된 오브젝트가 있으면 오브젝트 옵션윈도우를 활성화
	if (CDummy::DUMMY_END > m_eCurRadio &&
		0 <= m_eCurRadio &&
		0 < m_Objects[m_eCurRadio].size())
	{
		OBJECT_OPTIONSWINDOW->Set_CurrentDummy(m_eCurRadio, 
			dynamic_cast<CDummy*>(m_Objects[m_eCurRadio][m_iCurrentListIndex]));
	}
	else
	{
		OBJECT_OPTIONSWINDOW->Set_CurrentDummy(CDummy::DUMMY_END, nullptr);
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

	for (_uint i = 0; i < CDummy::DUMMY_END; ++i)
	{
		for (auto& iter : m_Objects[i])
		{
			Safe_Release(iter);
		}
		m_Objects[i].clear();

		for (auto& iter : m_ObjectNames[i])
		{
			Safe_Delete_Array(iter);
		}
		m_ObjectNames[i].clear();
	}
}
