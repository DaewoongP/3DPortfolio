#pragma once
#include "imgui.h"
#include "ImGuiFileDialog.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <windows.h>
#include "Tool_Function.h"

#define IMFILE        ImGuiFileDialog::Instance()

namespace Tool
{
    const unsigned int g_iWinSizeX = 1280;
    const unsigned int g_iWinSizeY = 720;

    enum LEVELID { LEVEL_TOOL, LEVEL_END };
}

extern HINSTANCE    g_hInst;
extern HWND         g_hWnd;
extern UINT         g_ResizeWidth;
extern UINT         g_ResizeHeight;

using namespace Tool;
using namespace ImGui;