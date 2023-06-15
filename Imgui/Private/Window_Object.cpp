#include "..\Public\Window_Object.h"
#include "GameInstance.h"
#include "Layer.h"

CWindow_Object::CWindow_Object()
{
}

HRESULT CWindow_Object::Initialize(void* pArg)
{
	m_vWindowPos = ImVec2(-870, g_iWinSizeY);
	m_vWindowSize = ImVec2(400, 300);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CLayer* pLayer = pGameInstance->Find_Layer(LEVEL_TOOL, TEXT("Layer_Tool"));
	pLayer->Get_AllGameObject();

	Safe_Release(pGameInstance);
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
	if (ImGui::ListBox("Current Objects", &m_iCurrentListIndex, m_Objects.data(), (_int)m_Objects.size()))
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
}
