#include "..\Public\Window_Object.h"
#include "GameInstance.h"
#include "Layer.h"
#include "GameObject.h"

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
