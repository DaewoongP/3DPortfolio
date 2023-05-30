#include "..\Public\ImWindow.h"
#include "GameInstance.h"

CImWindow::CImWindow()
    : m_pGameInstance(CGameInstance::GetInstance())
    , m_IsShow(true)
{
    Safe_AddRef(m_pGameInstance);
}

void CImWindow::Free(void)
{
    Safe_Release(m_pGameInstance);
}