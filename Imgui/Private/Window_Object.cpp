#include "..\Public\Window_Object.h"
#include "Layer.h"
#include "Dummy.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "ImWindow_Manager.h"
#include "Camera_Free.h"

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

	/*CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CLayer* pLayer = pGameInstance->Find_Layer(LEVEL_TOOL, TEXT("Layer_Tool"));
	m_Objects.reserve(pLayer->Get_AllGameObject().size());

	for (auto& pair : pLayer->Get_AllGameObject())
	{
		m_Objects.push_back(pair.second);
		Safe_AddRef(pair.second);
	}
	
	Safe_Release(pGameInstance);*/
	return S_OK;
}

void CWindow_Object::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
	Begin("Object", nullptr, m_WindowFlag);

	CurrentObjectListBox();

	End();
}

HRESULT CWindow_Object::CurrentObjectListBox()
{
	if (ImGui::ListBox("Current Objects", &m_iCurrentListIndex, m_ObjectNames.data(), (_int)m_ObjectNames.size()))
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);
		
		CDummy* pDummy = dynamic_cast<CDummy*>(m_Objects[m_iCurrentListIndex]);
		CCamera_Free* pCam = dynamic_cast<CCamera_Free*>(pGameInstance->Find_GameObject(LEVEL_TOOL, TEXT("Layer_Tool"), TEXT("GameObject_Camera_Free")));
		
		if (nullptr == pDummy ||
			nullptr == pCam)
		{
			Safe_Release(pGameInstance);
			return E_FAIL;
		}

		pGameInstance->Set_LastGameObject(m_Objects[m_iCurrentListIndex]);
		MODELWINDOW->Set_InputScale(pDummy->Get_PreToolScale());
		MODELWINDOW->Set_InputRotation(pDummy->Get_PreToolRotation());
		MODELWINDOW->Set_InputTransform(pDummy->Get_PreToolTransform());

		_float4 vTransform = pDummy->Get_PreToolTransform();
		vTransform.y += 15.f;
		vTransform.z -= 15.f;
		pCam->Set_CameraView(vTransform, pDummy->Get_PreToolTransform(), _float4(0.f, 1.f, 0.f, 0.f));

		Safe_Release(pGameInstance);
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
