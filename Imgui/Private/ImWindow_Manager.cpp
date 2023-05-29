#include "..\Public\ImWindow_Manager.h"

IMPLEMENT_SINGLETON(CImWindow_Manager)

CImWindow_Manager::CImWindow_Manager()
{
}

CImWindow* CImWindow_Manager::Get_ImWindow(wstring tag)
{
    return Find_Window(tag);
}

HRESULT CImWindow_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImGuiIO** pIO)
{
    // Show the window
    ::ShowWindow(g_hWnd, SW_SHOWDEFAULT);
    ::UpdateWindow(g_hWnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    *pIO = &io;
    m_pIO = *pIO;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    //g_io.ConfigViewportsNoAutoMerge = true;
    //g_io.ConfigViewportsNoTaskBarIcon = true;
    //g_io.ConfigViewportsNoDefaultParent = true;
    //g_io.ConfigDockingAlwaysTabBar = true;
    //g_io.ConfigDockingTransparentPayload = true;
    //g_io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;     // FIXME-DPI: Experimental. THIS CURRENTLY DOESN'T WORK AS EXPECTED. DON'T USE IN USER APP!
    //g_io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; // FIXME-DPI: Experimental.

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX11_Init(pDevice, pContext);

    return S_OK;
}

void CImWindow_Manager::Tick(_double dTimeDelta)
{
    for (auto ImWindow : m_ImWindows)
        ImWindow.second->Tick(dTimeDelta);
}

void CImWindow_Manager::Render()
{
    // Rendering
    ImGui::Render();

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    if (m_pIO->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

HRESULT CImWindow_Manager::Add_Window(wstring tag, CImWindow* pWindow)
{
    if (nullptr == pWindow)
        return E_FAIL;

    CImWindow* pImWindow = Find_Window(tag);

    if (nullptr != pImWindow)
        return E_FAIL;

    m_ImWindows.emplace(tag.c_str(), pWindow);

    return S_OK;
}

CImWindow* CImWindow_Manager::Find_Window(wstring tag)
{
    auto iter = find_if(m_ImWindows.begin(), m_ImWindows.end(), CTag_Finder(tag.c_str()));

    if (iter == m_ImWindows.end())
        return nullptr;

    return iter->second;
}

void CImWindow_Manager::Free(void)
{
    for (auto& ImWindow : m_ImWindows)
        Safe_Release(ImWindow.second);
}