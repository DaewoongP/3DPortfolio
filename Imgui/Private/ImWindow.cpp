#include "..\Public\ImWindow.h"
#include "GameInstance.h"
#include "ImWindow_Manager.h"

CImWindow::CImWindow(ImGuiIO* pIO)
    : m_pIO(pIO)
    , m_pGameInstance(CGameInstance::GetInstance())
    , m_pImWindow_Manager(CImWindow_Manager::GetInstance())
    , m_IsShow(true)
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pImWindow_Manager);
}

void CImWindow::Initialize(void* pArg)
{
}

void CImWindow::Free(void)
{
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pImWindow_Manager);
}