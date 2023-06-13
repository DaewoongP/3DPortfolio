#include "..\Public\ImWindow.h"

CImWindow::CImWindow()
    : m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
    ZEROMEM(&m_WindowFlag);
    ZEROMEM(&m_vWindowPos);
    ZEROMEM(&m_vWindowSize);
    m_WindowFlag |= ImGuiWindowFlags_NoMove;
    m_WindowFlag |= ImGuiWindowFlags_NoResize;
    m_WindowFlag |= ImGuiWindowFlags_NoCollapse;
}

void CImWindow::Tick(_double dTimeDelta)
{
    RECT rc;
    GetWindowRect(g_hWnd, &rc);

    ImVec2 vPos = ImVec2((_float)rc.right, (_float)rc.top);
    ImGui::SetNextWindowPos(vPos + m_vWindowPos);
    SetNextWindowSize(m_vWindowSize);
}

void CImWindow::Free(void)
{
    Safe_Release(m_pGameInstance);
}